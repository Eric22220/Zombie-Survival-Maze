/* ============================================================
 *  main.c  –  Game loop principal cu sistem de nivele
 *
 *  Stari:
 *    STATE_MENU            – ecran de start cu titlu
 *    STATE_LEVEL_SELECT    – lobby 1-10 cu nivele deblocate
 *    STATE_PLAYING         – jocul propriu-zis
 *    STATE_LEVEL_COMPLETE  – ai ajuns la EXIT
 *    STATE_LOSE            – te-au prins zombii
 *    STATE_GAME_COMPLETE   – ai terminat nivelul 10
 *
 *  Fluxul tipic:
 *    MENU → LEVEL_SELECT → PLAYING → LEVEL_COMPLETE → LEVEL_SELECT (sau next)
 *                                 → LOSE → LEVEL_SELECT (R = retry nivel)
 *
 *  Progresul (max_unlocked + levels_completed) e persistat in save.dat.
 * ============================================================ */

#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "core.h"
#include "save.h"
#include "maze/maze.h"
#include "world/player.h"
#include "world/zombie.h"
#include "world/level.h"
#include "world/powerup.h"
#include "gfx/renderer.h"

/* ---- Structura globala a jocului ---- */
typedef struct {
    /* Game state */
    GameState   state;
    int         current_level;    /* 1..MAX_LEVELS, valid in STATE_PLAYING */
    int         cursor_level;     /* selectia in lobby (1..MAX_LEVELS)     */
    int         should_quit;      /* 1 → ies din game loop                 */

    /* Persistenta */
    SaveData    save;

    /* Configul curent + entitati */
    LevelConfig cfg;
    Maze       *maze;
    Player      player;
    Zombie      zombies[MAX_ZOMBIES];
    int         keys_alive[MAX_KEYS];
    Powerup     powerups[MAX_POWERUPS];

    /* Power-up timers globale (separate de cele din Player) */
    float       freeze_timer;
    float       kill_all_timer;
} Game;

/* ------------------------------------------------------------ *
 *  spawn_zombies – distribuie zombii pe 3 zone verticale
 *
 *  Imparte grila in 3 coloane (stanga / mijloc / dreapta) si
 *  rotest zonele intre zombi (zone = i % 3). Astfel cu 7 zombi:
 *    3 stanga, 2 mijloc, 2 dreapta — vin din directii diferite.
 *
 *  Pentru fiecare zombie:
 *    1. Incearca 200 de pozitii random in zona alocata,
 *       pe celule FLOOR la distanta Manhattan > 8 de player
 *    2. Fallback: cauta oriunde in maze, distanta > 4
 *    3. Ultimate fallback: coltul opus al labirintului
 * ------------------------------------------------------------ */
static void spawn_zombies(Game *g) {
    int target = g->cfg.num_zombies;
    if (target > MAX_ZOMBIES) target = MAX_ZOMBIES;

    for (int i = 0; i < MAX_ZOMBIES; i++) {
        g->zombies[i].alive = 0;
    }

    int cols   = g->maze->cols;
    int rows   = g->maze->rows;
    int zone_w = cols / 3;
    int spawned = 0;

    for (int i = 0; i < target; i++) {
        int zone = i % 3;   /* 0=stanga, 1=mijloc, 2=dreapta */
        int xmin = zone * zone_w;
        int xmax = (zone == 2) ? (cols - 1) : ((zone + 1) * zone_w);

        /* Asiguram macar 2 celule de range in zona */
        if (xmax - xmin < 2) {
            xmax = xmin + 2;
            if (xmax >= cols) { xmax = cols - 1; xmin = xmax - 2; }
        }

        int found = 0;

        /* --- 1. Incearca in zona, distanta > 8 fata de player --- */
        for (int attempt = 0; attempt < 200 && !found; attempt++) {
            int range_x = (xmax - xmin) / 2;
            if (range_x < 1) range_x = 1;
            int rx = xmin + 1 + (rand() % range_x) * 2;
            int ry = 1 + (rand() % ((rows - 1) / 2)) * 2;

            if (rx < 1 || rx >= cols - 1) continue;
            if (g->maze->grid[ry][rx] != TILE_FLOOR) continue;

            int dist = abs(rx - g->player.pos.x) + abs(ry - g->player.pos.y);
            if (dist <= 8) continue;

            int dup = 0;
            for (int j = 0; j < spawned; j++) {
                if (g->zombies[j].pos.x == rx && g->zombies[j].pos.y == ry) {
                    dup = 1; break;
                }
            }
            if (dup) continue;

            zombie_init(&g->zombies[spawned], (Vec2i){rx, ry});
            spawned++;
            found = 1;
        }

        /* --- 2. Fallback: oriunde in maze, distanta > 4 --- */
        for (int attempt = 0; attempt < 500 && !found; attempt++) {
            int rx = 1 + (rand() % ((cols - 1) / 2)) * 2;
            int ry = 1 + (rand() % ((rows - 1) / 2)) * 2;

            if (g->maze->grid[ry][rx] != TILE_FLOOR) continue;

            int dist = abs(rx - g->player.pos.x) + abs(ry - g->player.pos.y);
            if (dist <= 4) continue;

            int dup = 0;
            for (int j = 0; j < spawned; j++) {
                if (g->zombies[j].pos.x == rx && g->zombies[j].pos.y == ry) {
                    dup = 1; break;
                }
            }
            if (dup) continue;

            zombie_init(&g->zombies[spawned], (Vec2i){rx, ry});
            spawned++;
            found = 1;
        }

        /* --- 3. Ultimate fallback: coltul opus --- */
        if (!found) {
            zombie_init(&g->zombies[spawned], (Vec2i){cols - 2, rows - 2});
            spawned++;
        }
    }
}

/* ------------------------------------------------------------ */
/*  start_level – construieste totul pentru nivelul dat        */
/* ------------------------------------------------------------ */
static void start_level(Game *g, int level) {
    /* Elibereaza labirintul anterior daca exista */
    if (g->maze != NULL) {
        maze_free(g->maze);
        g->maze = NULL;
    }

    g->cfg = level_get_config(level);
    g->current_level = level;

    /* Aloca + genereaza labirint cu dimensiunea din config */
    g->maze = maze_new(g->cfg.maze_dim, g->cfg.maze_dim);
    maze_generate(g->maze, (unsigned int)time(NULL), g->cfg.num_keys);
    maze_add_loops(g->maze, g->cfg.num_loops);

    /* Player la start */
    g->player = player_new(g->maze->start);

    /* Toate cheile vii */
    for (int i = 0; i < MAX_KEYS; i++) {
        g->keys_alive[i] = (i < g->maze->num_keys) ? 1 : 0;
    }

    /* Zombi */
    spawn_zombies(g);

    /* Power-uri pe jos */
    powerups_spawn(g->powerups, g->cfg.num_powerups, g->maze,
                   g->maze->start, g->maze->exit);

    /* Reset timer-e globale */
    g->freeze_timer   = 0.0f;
    g->kill_all_timer = 0.0f;

    g->state = STATE_PLAYING;
}

/* ------------------------------------------------------------ */
/*  apply_powerup – aplica efectul cand player calca pe el     */
/* ------------------------------------------------------------ */
static void apply_powerup(Game *g, PowerupType type) {
    switch (type) {
        case POWERUP_SPEED:
            g->player.speed_timer = POWERUP_DURATION;
            g->player.hold_timer  = 0.0f;   /* reset, ca primul pas in SPEED sa fie instant */
            break;

        case POWERUP_FREEZE:
            g->freeze_timer = POWERUP_DURATION;
            break;

        case POWERUP_HEAL:
            if (g->player.health < PLAYER_HEALTH) {
                g->player.health++;
            }
            break;

        case POWERUP_INVINCIBLE:
            g->player.invincible_pu_timer = POWERUP_DURATION;
            break;

        case POWERUP_KILL_ALL:
            g->kill_all_timer = POWERUP_DURATION;
            for (int i = 0; i < g->cfg.num_zombies; i++) {
                g->zombies[i].alive = 0;
            }
            break;

        default:
            break;
    }
}

/* ------------------------------------------------------------ */
/*  Update per frame – doar in STATE_PLAYING                   */
/* ------------------------------------------------------------ */
static void game_update(Game *g, float dt) {
    if (g->state != STATE_PLAYING) return;

    /* 1. Player – input + decrement timer-e personale */
    player_update(&g->player, g->maze, dt);

    /* 2. Update timer-e globale de powerups */
    if (g->freeze_timer   > 0.0f) { g->freeze_timer   -= dt; if (g->freeze_timer   < 0) g->freeze_timer   = 0; }
    if (g->kill_all_timer > 0.0f) {
        g->kill_all_timer -= dt;
        if (g->kill_all_timer <= 0.0f) {
            /* Reinvie zombii la pozitiile lor de spawn */
            g->kill_all_timer = 0.0f;
            for (int i = 0; i < g->cfg.num_zombies; i++) {
                g->zombies[i].pos        = g->zombies[i].spawn;
                g->zombies[i].alive      = 1;
                g->zombies[i].move_timer = 0.0f;
            }
        }
    }

    /* 3. Verifica daca player a calcat pe cheie */
    for (int i = 0; i < g->maze->num_keys; i++) {
        if (g->keys_alive[i] &&
            g->player.pos.x == g->maze->keys[i].x &&
            g->player.pos.y == g->maze->keys[i].y)
        {
            g->keys_alive[i] = 0;
            player_collect_key(&g->player);
        }
    }

    /* 4. Verifica daca player a calcat pe powerup */
    for (int i = 0; i < g->cfg.num_powerups; i++) {
        if (g->powerups[i].alive &&
            g->player.pos.x == g->powerups[i].pos.x &&
            g->player.pos.y == g->powerups[i].pos.y)
        {
            g->powerups[i].alive = 0;
            apply_powerup(g, g->powerups[i].type);
        }
    }

    /* 5. Conditie de victorie: pe EXIT cu toate cheile */
    if (g->player.pos.x == g->maze->exit.x &&
        g->player.pos.y == g->maze->exit.y &&
        g->player.keys_collected == g->maze->num_keys)
    {
        /* Salveaza progresul */
        save_mark_complete(&g->save, g->current_level);

        /* Daca era ultimul nivel → game complete, altfel level complete */
        if (g->current_level >= MAX_LEVELS) {
            g->state = STATE_GAME_COMPLETE;
        } else {
            g->state = STATE_LEVEL_COMPLETE;
        }
        return;
    }

    /* 6. Update zombi (BFS sau frozen) */
    int frozen = (g->freeze_timer > 0.0f) ? 1 : 0;
    for (int i = 0; i < g->cfg.num_zombies; i++) {
        zombie_update(&g->zombies[i], g->maze, g->player.pos,
                      dt, g->cfg.zombie_speed, frozen);
    }

    /* 7. Coliziune zombie - player */
    for (int i = 0; i < g->cfg.num_zombies; i++) {
        if (!g->zombies[i].alive) continue;

        if (g->zombies[i].pos.x == g->player.pos.x &&
            g->zombies[i].pos.y == g->player.pos.y)
        {
            /* Loveste doar daca player NU e invincibil (din nici un motiv) */
            if (!player_is_invincible(&g->player)) {
                g->player.health--;
                g->player.invincible_timer = INVINCIBLE_FRAMES;

                if (!player_is_alive(&g->player)) {
                    g->state = STATE_LOSE;
                    return;
                }
            }
        }
    }
}

/* ------------------------------------------------------------ */
/*  Draw per frame                                              */
/* ------------------------------------------------------------ */
static void game_draw(const Game *g) {
    BeginDrawing();
    ClearBackground((Color){12, 12, 18, 255});

    /* Scena de joc se randeaza sub overlay-uri */
    if (g->state == STATE_PLAYING       ||
        g->state == STATE_LEVEL_COMPLETE||
        g->state == STATE_LOSE          ||
        g->state == STATE_GAME_COMPLETE)
    {
        render_maze    (g->maze, g->keys_alive);
        render_exit    (g->maze, g->maze->exit,
                        g->player.keys_collected == g->maze->num_keys);
        render_powerups(g->maze, g->powerups, g->cfg.num_powerups);
        render_zombies (g->maze, g->zombies, g->cfg.num_zombies);
        render_player  (g->maze, &g->player);
        render_hud     (&g->player, &g->cfg,
                        g->freeze_timer, g->kill_all_timer);
    }

    /* Ecranele full-screen */
    if (g->state == STATE_MENU) {
        render_menu();
    } else if (g->state == STATE_LEVEL_SELECT) {
        render_level_select(&g->save, g->cursor_level);
    } else {
        render_overlay(g->state, g->current_level);
    }

    EndDrawing();
}

/* ------------------------------------------------------------ */
/*  Input pentru tranzitii de stare (NU pentru gameplay)        */
/* ------------------------------------------------------------ */
static void handle_state_input(Game *g) {
    switch (g->state) {

        case STATE_MENU:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                g->state = STATE_LEVEL_SELECT;
                g->cursor_level = g->save.max_unlocked;
            }
            /* Q = inchide jocul (Q nu se confunda cu alte litere in font-ul Raylib;
               X arata identic cu H la rezolutia asta) */
            if (IsKeyPressed(KEY_Q)) {
                g->should_quit = 1;
            }
            break;

        case STATE_LEVEL_SELECT: {
            /* Navigare cu sageti, grid 5x2 */
            if (IsKeyPressed(KEY_RIGHT)) {
                if (g->cursor_level % 5 != 0 && g->cursor_level < MAX_LEVELS)
                    g->cursor_level++;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                if ((g->cursor_level - 1) % 5 != 0)
                    g->cursor_level--;
            }
            if (IsKeyPressed(KEY_DOWN)) {
                if (g->cursor_level + 5 <= MAX_LEVELS)
                    g->cursor_level += 5;
            }
            if (IsKeyPressed(KEY_UP)) {
                if (g->cursor_level - 5 >= 1)
                    g->cursor_level -= 5;
            }
            /* ENTER porneste nivelul daca e deblocat */
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (g->cursor_level <= g->save.max_unlocked) {
                    start_level(g, g->cursor_level);
                }
            }
            /* ESC inapoi la meniu */
            if (IsKeyPressed(KEY_ESCAPE)) {
                g->state = STATE_MENU;
            }
            break;
        }

        case STATE_PLAYING:
            /* ESC = inapoi la lobby (abandoneaza nivel) */
            if (IsKeyPressed(KEY_ESCAPE)) {
                g->state = STATE_LEVEL_SELECT;
                g->cursor_level = g->current_level;
            }
            break;

        case STATE_LEVEL_COMPLETE:
            /* ENTER = nivel urmator daca exista, altfel inapoi la lobby */
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                int next = g->current_level + 1;
                if (next <= MAX_LEVELS) {
                    start_level(g, next);
                } else {
                    g->state = STATE_LEVEL_SELECT;
                    g->cursor_level = MAX_LEVELS;
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                g->state = STATE_LEVEL_SELECT;
                g->cursor_level = g->current_level;
            }
            break;

        case STATE_LOSE:
            /* R = retry, ESC = lobby */
            if (IsKeyPressed(KEY_R)) {
                start_level(g, g->current_level);
            }
            if (IsKeyPressed(KEY_ESCAPE) ||
                IsKeyPressed(KEY_ENTER)  ||
                IsKeyPressed(KEY_SPACE))
            {
                g->state = STATE_LEVEL_SELECT;
                g->cursor_level = g->current_level;
            }
            break;

        case STATE_GAME_COMPLETE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                g->state = STATE_LEVEL_SELECT;
                g->cursor_level = MAX_LEVELS;
            }
            break;
    }
}

/* ------------------------------------------------------------ */
/*  main                                                        */
/* ------------------------------------------------------------ */
int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Zombie Maze  |  10 nivele de supravietuire");
    SetTargetFPS(FPS);
    SetExitKey(0);     /* dezactivam ESC = quit automat – il gestionam noi */

    /* Initializam Game */
    Game g;
    g.state          = STATE_MENU;
    g.current_level  = 1;
    g.should_quit    = 0;
    g.maze           = NULL;
    g.freeze_timer   = 0.0f;
    g.kill_all_timer = 0.0f;
    for (int i = 0; i < MAX_ZOMBIES; i++) g.zombies[i].alive = 0;
    for (int i = 0; i < MAX_POWERUPS; i++) g.powerups[i].alive = 0;
    for (int i = 0; i < MAX_KEYS; i++) g.keys_alive[i] = 0;

    /* Incarcam progresul */
    g.save = save_load();
    g.cursor_level = g.save.max_unlocked;

    /* ---- Game Loop principal ---- */
    while (!WindowShouldClose() && !g.should_quit) {
        float dt = GetFrameTime();

        handle_state_input(&g);
        game_update(&g, dt);
        game_draw  (&g);
    }

    /* ---- Cleanup ---- */
    if (g.maze != NULL) {
        maze_free(g.maze);
    }
    CloseWindow();
    return 0;
}
