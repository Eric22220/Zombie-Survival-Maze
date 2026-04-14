/* ============================================================
 *  main.c  –  Game loop principal + gestionarea starii globale
 *
 *  Structura unui frame:
 *    1. Input / Update  (player, colectare chei, zombies)
 *    2. Draw            (maze, entitati, HUD, overlay)
 *
 *  Toata memoria alocata dinamic (labirint, coada BFS internă)
 *  e eliberata corect la inchiderea jocului.
 * ============================================================ */

#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "core.h"
#include "maze/maze.h"
#include "world/player.h"
#include "world/zombie.h"
#include "gfx/renderer.h"

/* ---- Structura globala a jocului ---- */
typedef struct {
    Maze     *maze;
    Player    player;
    Zombie    zombies[NUM_ZOMBIES];
    int       keys_alive[NUM_KEYS];   /* 1 = pe jos, 0 = colectata          */
    GameState state;
} Game;

/* ------------------------------------------------------------ */
/*  Spawn zombies departe de jucator                            */
/* ------------------------------------------------------------ */
static void spawn_zombies(Game *g) {
    int spawned = 0;

    /* Prima trecere: cautam celule floor departe de start (dist > 8) */
    for (int r = g->maze->rows - 2; r > 0 && spawned < NUM_ZOMBIES; r -= 2) {
        for (int c = g->maze->cols - 2; c > 0 && spawned < NUM_ZOMBIES; c -= 2) {
            if (g->maze->grid[r][c] != TILE_FLOOR) continue;

            int dist = abs(c - g->player.pos.x) + abs(r - g->player.pos.y);
            if (dist <= 8) continue;

            /* Verificam sa nu mai fie un zombie acolo */
            int dup = 0;
            for (int i = 0; i < spawned; i++) {
                if (g->zombies[i].pos.x == c && g->zombies[i].pos.y == r) {
                    dup = 1; break;
                }
            }
            if (!dup) {
                zombie_init(&g->zombies[spawned], (Vec2i){c, r});
                spawned++;
            }
        }
    }

    /* A doua trecere: orice celula floor la dist > 4 (fallback) */
    for (int r = 1; r < g->maze->rows && spawned < NUM_ZOMBIES; r += 2) {
        for (int c = 1; c < g->maze->cols && spawned < NUM_ZOMBIES; c += 2) {
            if (g->maze->grid[r][c] != TILE_FLOOR) continue;

            int dist = abs(c - g->player.pos.x) + abs(r - g->player.pos.y);
            if (dist <= 4) continue;

            int dup = 0;
            for (int i = 0; i < spawned; i++) {
                if (g->zombies[i].pos.x == c && g->zombies[i].pos.y == r) {
                    dup = 1; break;
                }
            }
            if (!dup) {
                zombie_init(&g->zombies[spawned], (Vec2i){c, r});
                spawned++;
            }
        }
    }

    /* Fallback de urgenta: coltul opus */
    for (int i = spawned; i < NUM_ZOMBIES; i++) {
        zombie_init(&g->zombies[i],
                    (Vec2i){g->maze->cols - 2, g->maze->rows - 2});
    }
}

/* ------------------------------------------------------------ */
/*  Initializare / Restart joc                                  */
/* ------------------------------------------------------------ */
static void game_init(Game *g) {
    /* Eliberam labirintul anterior daca exista */
    if (g->maze != NULL) {
        maze_free(g->maze);
    }

    g->maze = maze_new(MAZE_ROWS, MAZE_COLS);
    maze_generate(g->maze, (unsigned int)time(NULL));

    g->player = player_new(g->maze->start);

    for (int i = 0; i < NUM_KEYS; i++) {
        g->keys_alive[i] = 1;
    }

    spawn_zombies(g);

    g->state = STATE_PLAYING;
}

/* ------------------------------------------------------------ */
/*  Update per frame                                            */
/* ------------------------------------------------------------ */
static void game_update(Game *g, float dt) {
    if (g->state != STATE_PLAYING) return;

    /* 1. Miscam jucatorul pe baza inputului */
    player_update(&g->player, g->maze);

    /* 2. Verificam daca jucatorul a calcat pe o cheie */
    for (int i = 0; i < NUM_KEYS; i++) {
        if (g->keys_alive[i] &&
            g->player.pos.x == g->maze->keys[i].x &&
            g->player.pos.y == g->maze->keys[i].y)
        {
            g->keys_alive[i] = 0;
            player_collect_key(&g->player);
        }
    }

    /* 3. Verificam conditia de victorie:
          jucatorul pe EXIT cu TOATE cheile colectate */
    if (g->player.pos.x == g->maze->exit.x &&
        g->player.pos.y == g->maze->exit.y &&
        g->player.keys_collected == NUM_KEYS)
    {
        g->state = STATE_WIN;
        return;
    }

    /* 4. Actualizam zombie-ii (BFS spre jucator) */
    for (int i = 0; i < NUM_ZOMBIES; i++) {
        zombie_update(&g->zombies[i], g->maze, g->player.pos, dt);
    }

    /* 5. Verificam coliziunea zombie – jucator */
    for (int i = 0; i < NUM_ZOMBIES; i++) {
        if (!g->zombies[i].alive) continue;

        if (g->zombies[i].pos.x == g->player.pos.x &&
            g->zombies[i].pos.y == g->player.pos.y)
        {
            /* Loveste doar daca jucatorul NU e invincibil */
            if (g->player.invincible_timer == 0) {
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

    render_maze(g->maze, g->keys_alive);
    render_exit(g->maze->exit, g->player.keys_collected == NUM_KEYS);
    render_player(&g->player);
    render_zombies(g->zombies, NUM_ZOMBIES);
    render_hud(&g->player, NUM_KEYS);
    render_overlay(g->state);

    EndDrawing();
}

/* ------------------------------------------------------------ */
/*  main                                                        */
/* ------------------------------------------------------------ */
int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Zombie Maze  |  Supravietuieste!");
    SetTargetFPS(FPS);

    /* Initializam jocul cu un labirint de previzualizare pentru meniu */
    Game g = {0};
    g.state = STATE_MENU;
    g.maze  = maze_new(MAZE_ROWS, MAZE_COLS);
    maze_generate(g.maze, 1337u);
    g.player = player_new(g.maze->start);
    for (int i = 0; i < NUM_KEYS; i++) g.keys_alive[i] = 1;
    spawn_zombies(&g);

    /* ---- Game Loop principal ---- */
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* Tranzitii de stare */
        if (g.state == STATE_MENU && IsKeyPressed(KEY_ENTER)) {
            game_init(&g);
        } else if ((g.state == STATE_WIN || g.state == STATE_LOSE)
                   && IsKeyPressed(KEY_R)) {
            game_init(&g);
        }

        game_update(&g, dt);
        game_draw(&g);
    }

    /* ---- Cleanup: eliberam toata memoria alocata ---- */
    maze_free(g.maze);
    CloseWindow();
    return 0;
}
