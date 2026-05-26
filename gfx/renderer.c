/* ============================================================
 *  gfx/renderer.c  –  Desenarea intregii scene cu Raylib
 *
 *  Toate functiile care deseneaza pe grila aplica un offset
 *  pentru a centra labirintul in fereastra (utile cand
 *  dim labirintului < MAX_MAZE_DIM).
 * ============================================================ */

#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "gfx/renderer.h"

/* --- Paleta de culori a jocului --- */
static const Color COL_WALL          = { 25,  25,  38, 255};
static const Color COL_FLOOR         = {190, 185, 170, 255};
static const Color COL_WALL_BORDER   = { 15,  15,  25, 255};
static const Color COL_EXIT_LOCKED   = { 90,  90, 110, 255};
static const Color COL_EXIT_UNLOCKED = { 60, 210,  80, 255};
static const Color COL_KEY           = {255, 200,  10, 255};
static const Color COL_PLAYER        = { 60, 130, 240, 255};
static const Color COL_PLAYER_BLINK  = {140, 200, 255, 255};
static const Color COL_PLAYER_INV    = {255, 220,  80, 255};   /* invincibil de la powerup */
static const Color COL_ZOMBIE        = {180,  20,  20, 255};
static const Color COL_ZOMBIE_EYE    = { 50, 240,  50, 255};
static const Color COL_HUD_BG        = { 18,  18,  28, 255};
static const Color COL_BG            = { 12,  12,  18, 255};

/* Culori per tip de powerup */
static Color powerup_color(PowerupType t) {
    switch (t) {
        case POWERUP_SPEED:      return (Color){ 80, 220, 255, 255};   /* cyan      */
        case POWERUP_FREEZE:     return (Color){180, 220, 255, 255};   /* alb-albastrui */
        case POWERUP_HEAL:       return (Color){255, 100, 150, 255};   /* roz       */
        case POWERUP_INVINCIBLE: return (Color){255, 220,  80, 255};   /* galben    */
        case POWERUP_KILL_ALL:   return (Color){255, 120,  40, 255};   /* portocaliu*/
        default:                 return WHITE;
    }
}

/* ============================================================
 *  Offset pentru centrare in fereastra (cand maze < MAX)
 * ============================================================ */

int maze_offset_x(const Maze *m) {
    return (SCREEN_W - m->cols * TILE_SIZE) / 2;
}
int maze_offset_y(const Maze *m) {
    /* Centram in zona deasupra HUD-ului */
    int usable_h = SCREEN_H - HUD_HEIGHT;
    return (usable_h - m->rows * TILE_SIZE) / 2;
}

/* ============================================================
 *  render_maze – pereti, podea, chei
 * ============================================================ */

void render_maze(const Maze *m, const int keys_alive[MAX_KEYS]) {
    int ox = maze_offset_x(m);
    int oy = maze_offset_y(m);

    for (int r = 0; r < m->rows; r++) {
        for (int c = 0; c < m->cols; c++) {
            int px = ox + c * TILE_SIZE;
            int py = oy + r * TILE_SIZE;

            if (m->grid[r][c] == TILE_WALL) {
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, COL_WALL);
                DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, COL_WALL_BORDER);
            } else {
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, COL_FLOOR);
            }
        }
    }

    /* Chei vii deasupra podelei */
    for (int i = 0; i < m->num_keys; i++) {
        if (!keys_alive[i]) continue;

        int kx = ox + m->keys[i].x * TILE_SIZE;
        int ky = oy + m->keys[i].y * TILE_SIZE;
        int pad = TILE_SIZE / 5;

        DrawRectangle(kx + pad, ky + pad,
                      TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, COL_KEY);
        DrawRectangleLines(kx + pad, ky + pad,
                           TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, ORANGE);
        DrawText("K", kx + pad + 3, ky + pad + 2, 14, BLACK);
    }
}

/* ============================================================
 *  render_exit
 * ============================================================ */

void render_exit(const Maze *m, Vec2i exit_pos, int unlocked) {
    int ox = maze_offset_x(m);
    int oy = maze_offset_y(m);
    int px = ox + exit_pos.x * TILE_SIZE;
    int py = oy + exit_pos.y * TILE_SIZE;
    Color c = unlocked ? COL_EXIT_UNLOCKED : COL_EXIT_LOCKED;

    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, c);
    DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, WHITE);

    const char *label = unlocked ? "OUT" : " X";
    DrawText(label, px + 4, py + TILE_SIZE / 3, 11, unlocked ? BLACK : DARKGRAY);
}

/* ============================================================
 *  render_player
 * ============================================================ */

void render_player(const Maze *m, const Player *p) {
    int ox = maze_offset_x(m);
    int oy = maze_offset_y(m);
    int px = ox + p->pos.x * TILE_SIZE;
    int py = oy + p->pos.y * TILE_SIZE;
    int pad = 4;

    /* Culoarea corpului depinde de starea jucatorului */
    Color body;
    if (p->invincible_pu_timer > 0.0f) {
        /* Invincibilitate de la powerup – galben pulsing */
        body = (((int)(p->invincible_pu_timer * 10.0f)) % 2 == 0) ? COL_PLAYER_INV : COL_PLAYER;
    } else if (p->invincible_timer > 0) {
        /* Invincibilitate post-hit – blink */
        body = ((p->invincible_timer / 6) % 2 == 0) ? COL_PLAYER : COL_PLAYER_BLINK;
    } else {
        body = COL_PLAYER;
    }

    DrawRectangle(px + pad, py + pad, TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, body);
    DrawRectangleLines(px + pad, py + pad, TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, WHITE);

    /* Ochi */
    int eye_y = py + pad + 6;
    DrawCircle(px + pad + 6,        eye_y, 3, WHITE);
    DrawCircle(px + TILE_SIZE - 10, eye_y, 3, WHITE);
    DrawCircle(px + pad + 7,        eye_y, 1, DARKBLUE);
    DrawCircle(px + TILE_SIZE - 9,  eye_y, 1, DARKBLUE);
}

/* ============================================================
 *  render_zombies
 * ============================================================ */

void render_zombies(const Maze *m, const Zombie *zombies, int count) {
    int ox = maze_offset_x(m);
    int oy = maze_offset_y(m);

    for (int i = 0; i < count; i++) {
        if (!zombies[i].alive) continue;

        int zx  = ox + zombies[i].pos.x * TILE_SIZE;
        int zy  = oy + zombies[i].pos.y * TILE_SIZE;
        int pad = 4;

        DrawRectangle(zx + pad, zy + pad,
                      TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, COL_ZOMBIE);
        DrawRectangleLines(zx + pad, zy + pad,
                           TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, MAROON);

        int eye_y = zy + pad + 6;
        DrawCircle(zx + pad + 6,        eye_y, 3, COL_ZOMBIE_EYE);
        DrawCircle(zx + TILE_SIZE - 10, eye_y, 3, COL_ZOMBIE_EYE);

        for (int t = 0; t < 3; t++) {
            int tx = zx + pad + 4 + t * 6;
            DrawLine(tx, zy + TILE_SIZE - pad - 6,
                     tx, zy + TILE_SIZE - pad - 1, WHITE);
        }
    }
}

/* ============================================================
 *  render_powerups
 * ============================================================ */

void render_powerups(const Maze *m, const Powerup *pus, int count) {
    int ox = maze_offset_x(m);
    int oy = maze_offset_y(m);

    /* Animatie de pulsing bazata pe timpul jocului */
    float t = (float)GetTime();
    float scale = 0.85f + 0.15f * sinf(t * 4.0f);

    for (int i = 0; i < count; i++) {
        if (!pus[i].alive) continue;

        int cx = ox + pus[i].pos.x * TILE_SIZE + TILE_SIZE / 2;
        int cy = oy + pus[i].pos.y * TILE_SIZE + TILE_SIZE / 2;
        int rad = (int)((TILE_SIZE / 2 - 5) * scale);

        Color col = powerup_color(pus[i].type);

        /* Cercul de fundal cu gradient simulat (2 cercuri concentrice) */
        DrawCircle(cx, cy, rad + 2, (Color){col.r, col.g, col.b, 80});
        DrawCircle(cx, cy, rad,      col);
        DrawCircleLines(cx, cy, rad, WHITE);

        /* Glifa centrala */
        const char *g = powerup_glyph(pus[i].type);
        int tw = MeasureText(g, 16);
        DrawText(g, cx - tw / 2, cy - 8, 16, BLACK);
    }
}

/* ============================================================
 *  render_hud – HP, chei, nivel, power-uri active
 * ============================================================ */

void render_hud(const Player *p, const LevelConfig *cfg,
                float freeze_timer, float kill_all_timer)
{
    int hud_y = SCREEN_H - HUD_HEIGHT;

    /* Fundal HUD */
    DrawRectangle(0, hud_y, SCREEN_W, HUD_HEIGHT, COL_HUD_BG);
    DrawLine(0, hud_y, SCREEN_W, hud_y, DARKGRAY);

    /* --- Viata --- */
    DrawText("HP:", 10, hud_y + 8, 20, LIGHTGRAY);
    for (int i = 0; i < PLAYER_HEALTH; i++) {
        Color hcol = (i < p->health) ? RED : DARKGRAY;
        DrawText("<3", 48 + i * 28, hud_y + 8, 20, hcol);
    }

    /* --- Chei --- */
    char keys_buf[32];
    snprintf(keys_buf, sizeof(keys_buf), "Chei: %d / %d",
             p->keys_collected, cfg->num_keys);
    DrawText(keys_buf, 160, hud_y + 8, 20, YELLOW);

    /* --- Nivel --- */
    char lvl_buf[32];
    snprintf(lvl_buf, sizeof(lvl_buf), "NIVEL %d / %d", cfg->level, MAX_LEVELS);
    DrawText(lvl_buf, 320, hud_y + 8, 20, SKYBLUE);

    /* --- Power-uri active (timer-e in dreapta) --- */
    int px = 480;
    int py = hud_y + 8;
    if (p->speed_timer > 0.0f) {
        char buf[32];
        snprintf(buf, sizeof(buf), "VITEZA %.1fs", p->speed_timer);
        DrawText(buf, px, py, 18, (Color){ 80, 220, 255, 255});
        py += 22;
    }
    if (p->invincible_pu_timer > 0.0f) {
        char buf[32];
        snprintf(buf, sizeof(buf), "INVINCIBIL %.1fs", p->invincible_pu_timer);
        DrawText(buf, px, py, 18, (Color){255, 220, 80, 255});
        py += 22;
    }
    if (freeze_timer > 0.0f) {
        char buf[32];
        snprintf(buf, sizeof(buf), "INGHET %.1fs", freeze_timer);
        DrawText(buf, px + 180, hud_y + 8, 18, (Color){180, 220, 255, 255});
    }
    if (kill_all_timer > 0.0f) {
        char buf[32];
        snprintf(buf, sizeof(buf), "PURGE %.1fs", kill_all_timer);
        DrawText(buf, px + 180, hud_y + 30, 18, (Color){255, 120, 40, 255});
    }

    /* --- Hint taste (jos) --- */
    DrawText("WASD/Sageti = miscare    ESC = inapoi la lobby",
             10, hud_y + 52, 14, DARKGRAY);
}

/* ============================================================
 *  render_menu – ecranul de start
 * ============================================================ */

void render_menu(void) {
    ClearBackground(COL_BG);

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    /* Titlu mare */
    const char *title = "ZOMBIE MAZE";
    int tw = MeasureText(title, 72);
    DrawText(title, cx - tw / 2, cy - 60, 72, ORANGE);

    /* Hint principal */
    const char *hint = "ENTER = Selecteaza nivel    Q = Iesire";
    int hw = MeasureText(hint, 20);
    DrawText(hint, cx - hw / 2, cy + 30, 20, WHITE);
}

/* ============================================================
 *  render_level_select – lobby cu nivelele 1-10
 * ============================================================ */

void render_level_select(const SaveData *s, int cursor) {
    ClearBackground(COL_BG);

    /* Titlu */
    const char *title = "SELECTEAZA NIVELUL";
    int tw = MeasureText(title, 40);
    DrawText(title, (SCREEN_W - tw) / 2, 60, 40, ORANGE);

    /* Grid 5x2 cu butoanele 1-10 */
    int box_size = 100;
    int box_gap  = 24;
    int cols     = 5;
    int rows     = 2;

    int grid_w = cols * box_size + (cols - 1) * box_gap;
    int grid_h = rows * box_size + (rows - 1) * box_gap;
    int start_x = (SCREEN_W - grid_w) / 2;
    int start_y = 160;

    for (int i = 0; i < MAX_LEVELS; i++) {
        int level   = i + 1;
        int col_idx = i % cols;
        int row_idx = i / cols;

        int bx = start_x + col_idx * (box_size + box_gap);
        int by = start_y + row_idx * (box_size + box_gap);

        int unlocked  = (level <= s->max_unlocked);
        int completed = s->levels_completed[i];
        int selected  = (level == cursor);

        /* Culoare fundal */
        Color bg;
        if      (!unlocked)  bg = (Color){ 40,  40,  50, 255};
        else if (completed)  bg = (Color){ 40, 130,  60, 255};
        else                 bg = (Color){ 50,  70, 120, 255};

        DrawRectangle(bx, by, box_size, box_size, bg);

        /* Border – galben gros daca selectat */
        if (selected) {
            DrawRectangleLinesEx((Rectangle){bx - 2, by - 2,
                                              box_size + 4, box_size + 4}, 3, ORANGE);
        } else {
            DrawRectangleLines(bx, by, box_size, box_size, GRAY);
        }

        /* Numarul nivelului */
        char num_buf[8];
        snprintf(num_buf, sizeof(num_buf), "%d", level);
        int nw = MeasureText(num_buf, 48);
        Color text_col = unlocked ? WHITE : (Color){100, 100, 110, 255};
        DrawText(num_buf, bx + (box_size - nw) / 2, by + 20, 48, text_col);

        /* Icon de stare jos */
        if (!unlocked) {
            const char *lock = "BLOCAT";
            int lw = MeasureText(lock, 14);
            DrawText(lock, bx + (box_size - lw) / 2, by + box_size - 22, 14, GRAY);
        } else if (completed) {
            const char *ok = "TERMINAT";
            int ow = MeasureText(ok, 14);
            DrawText(ok, bx + (box_size - ow) / 2, by + box_size - 22, 14, LIME);
        } else {
            const char *go = "DESCHIS";
            int gw = MeasureText(go, 14);
            DrawText(go, bx + (box_size - gw) / 2, by + box_size - 22, 14, SKYBLUE);
        }
    }

    /* Info despre nivelul selectat */
    LevelConfig cfg = level_get_config(cursor);
    char info[128];
    snprintf(info, sizeof(info),
             "Nivel %d:  %dx%d  -  %d zombi  -  %d chei  -  viteza %.2fs",
             cfg.level, cfg.maze_dim, cfg.maze_dim,
             cfg.num_zombies, cfg.num_keys, cfg.zombie_speed);
    int iw = MeasureText(info, 18);
    DrawText(info, (SCREEN_W - iw) / 2, start_y + grid_h + 30, 18, LIGHTGRAY);

    /* Hint taste */
    const char *hint = "Sageti = navigare    ENTER = start    ESC = inapoi";
    int hw = MeasureText(hint, 18);
    DrawText(hint, (SCREEN_W - hw) / 2, SCREEN_H - 60, 18, GRAY);

    /* Progres total */
    int total_done = 0;
    for (int i = 0; i < MAX_LEVELS; i++) total_done += s->levels_completed[i];
    char prog[48];
    snprintf(prog, sizeof(prog), "Progres: %d / %d nivele terminate", total_done, MAX_LEVELS);
    int pw = MeasureText(prog, 16);
    DrawText(prog, (SCREEN_W - pw) / 2, SCREEN_H - 30, 16, DARKGRAY);
}

/* ============================================================
 *  render_overlay – ecrane semi-transparente peste joc
 * ============================================================ */

void render_overlay(GameState state, int current_level) {
    if (state == STATE_PLAYING || state == STATE_MENU || state == STATE_LEVEL_SELECT)
        return;

    /* Fundal semi-transparent peste joc */
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 185});

    const char *title    = "";
    const char *subtitle = "";
    const char *hint     = "";
    Color title_col      = WHITE;

    char sub_buf[96];

    switch (state) {
        case STATE_LEVEL_COMPLETE:
            title = "NIVEL COMPLETAT!";
            snprintf(sub_buf, sizeof(sub_buf),
                     "Ai terminat nivelul %d / %d", current_level, MAX_LEVELS);
            subtitle  = sub_buf;
            hint      = (current_level < MAX_LEVELS)
                        ? "ENTER = nivelul urmator    ESC = lobby"
                        : "ENTER = continua";
            title_col = LIME;
            break;

        case STATE_LOSE:
            title     = "AI MURIT...";
            subtitle  = "Zombii te-au prins.";
            hint      = "R = reincearca    ESC = inapoi la lobby";
            title_col = RED;
            break;

        case STATE_GAME_COMPLETE:
            title     = "FELICITARI!";
            subtitle  = "Ai terminat toate cele 10 nivele.";
            hint      = "ENTER = inapoi la lobby    ESC = iesire";
            title_col = GOLD;
            break;

        default:
            break;
    }

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    /* Pentru GAME_COMPLETE adaugam si un mesaj festiv */
    if (state == STATE_GAME_COMPLETE) {
        const char *trophy = "*** *** ***";
        int trw = MeasureText(trophy, 40);
        DrawText(trophy, cx - trw / 2, cy - 150, 40, GOLD);
    }

    /* Titlu principal */
    int tw = MeasureText(title, 56);
    DrawText(title, cx - tw / 2, cy - 70, 56, title_col);

    /* Subtitlu */
    int sw = MeasureText(subtitle, 24);
    DrawText(subtitle, cx - sw / 2, cy, 24, LIGHTGRAY);

    /* Hint taste */
    int hw = MeasureText(hint, 18);
    DrawText(hint, cx - hw / 2, cy + 50, 18, GRAY);
}
