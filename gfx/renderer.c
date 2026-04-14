/* ============================================================
 *  gfx/renderer.c  –  Desenarea intregii scene cu Raylib
 * ============================================================ */

#include <raylib.h>
#include <stdio.h>
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
static const Color COL_ZOMBIE        = {180,  20,  20, 255};
static const Color COL_ZOMBIE_EYE    = { 50, 240,  50, 255};
static const Color COL_HUD_BG        = { 18,  18,  28, 255};

/* ------------------------------------------------------------ */

void render_maze(const Maze *m, const int keys_alive[NUM_KEYS]) {
    for (int r = 0; r < m->rows; r++) {
        for (int c = 0; c < m->cols; c++) {
            int px = c * TILE_SIZE;
            int py = r * TILE_SIZE;

            if (m->grid[r][c] == TILE_WALL) {
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, COL_WALL);
                /* Bordura subtila pentru adancime vizuala */
                DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, COL_WALL_BORDER);
            } else {
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, COL_FLOOR);
            }
        }
    }

    /* Desenam cheile deasupra podelei */
    for (int i = 0; i < NUM_KEYS; i++) {
        if (!keys_alive[i]) continue;

        int kx = m->keys[i].x * TILE_SIZE;
        int ky = m->keys[i].y * TILE_SIZE;
        int pad = TILE_SIZE / 5;

        /* Corp cheie */
        DrawRectangle(kx + pad, ky + pad,
                      TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, COL_KEY);
        /* Contur */
        DrawRectangleLines(kx + pad, ky + pad,
                           TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, ORANGE);
        /* Simbolul "K" */
        DrawText("K", kx + pad + 3, ky + pad + 2, 14, BLACK);
    }
}

void render_exit(Vec2i exit_pos, int unlocked) {
    int px  = exit_pos.x * TILE_SIZE;
    int py  = exit_pos.y * TILE_SIZE;
    Color c = unlocked ? COL_EXIT_UNLOCKED : COL_EXIT_LOCKED;

    DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, c);
    DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, WHITE);

    const char *label = unlocked ? "OUT" : "  X";
    DrawText(label, px + 4, py + TILE_SIZE / 3, 11, unlocked ? BLACK : DARKGRAY);
}

void render_player(const Player *p) {
    int px  = p->pos.x * TILE_SIZE;
    int py  = p->pos.y * TILE_SIZE;
    int pad = 4;

    /* Efect de blink cat timp e invincibil */
    Color body = ((p->invincible_timer / 6) % 2 == 0) ? COL_PLAYER : COL_PLAYER_BLINK;

    /* Corp */
    DrawRectangle(px + pad, py + pad, TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, body);
    DrawRectangleLines(px + pad, py + pad, TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, WHITE);

    /* Ochi */
    int eye_y = py + pad + 6;
    DrawCircle(px + pad + 6,        eye_y, 3, WHITE);
    DrawCircle(px + TILE_SIZE - 10, eye_y, 3, WHITE);
    DrawCircle(px + pad + 7,        eye_y, 1, DARKBLUE);
    DrawCircle(px + TILE_SIZE - 9,  eye_y, 1, DARKBLUE);
}

void render_zombies(const Zombie *zombies, int count) {
    for (int i = 0; i < count; i++) {
        if (!zombies[i].alive) continue;

        int zx  = zombies[i].pos.x * TILE_SIZE;
        int zy  = zombies[i].pos.y * TILE_SIZE;
        int pad = 4;

        /* Corp zombie – dreptunghi rosu intunecat */
        DrawRectangle(zx + pad, zy + pad,
                      TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, COL_ZOMBIE);
        DrawRectangleLines(zx + pad, zy + pad,
                           TILE_SIZE - pad * 2, TILE_SIZE - pad * 2, MAROON);

        /* Ochi verzi stralucitori */
        int eye_y = zy + pad + 6;
        DrawCircle(zx + pad + 6,        eye_y, 3, COL_ZOMBIE_EYE);
        DrawCircle(zx + TILE_SIZE - 10, eye_y, 3, COL_ZOMBIE_EYE);

        /* "Dintii" – liniute albe in jos */
        for (int t = 0; t < 3; t++) {
            int tx = zx + pad + 4 + t * 6;
            DrawLine(tx, zy + TILE_SIZE - pad - 6,
                     tx, zy + TILE_SIZE - pad - 1, WHITE);
        }
    }
}

void render_hud(const Player *p, int total_keys) {
    int hud_y = MAZE_ROWS * TILE_SIZE;

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
    snprintf(keys_buf, sizeof(keys_buf), "Chei: %d / %d", p->keys_collected, total_keys);
    DrawText(keys_buf, 160, hud_y + 8, 20, YELLOW);

    /* --- Hint taste --- */
    DrawText("WASD / Sageti = miscare   R = restart   ESC = iesire",
             10, hud_y + 36, 14, DARKGRAY);
}

void render_overlay(GameState state) {
    if (state == STATE_PLAYING) return;

    /* Fundal semi-transparent */
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 175});

    const char *title    = "";
    const char *subtitle = "";
    const char *hint     = "";
    Color title_col      = WHITE;

    switch (state) {
        case STATE_MENU:
            title     = "ZOMBIE MAZE";
            subtitle  = "Aduna toate cheile si ajunge la EXIT!";
            hint      = "ENTER = Incepe   ESC = Iesire";
            title_col = ORANGE;
            break;
        case STATE_WIN:
            title     = "AI SCAPAT!";
            subtitle  = "Felicitari, supravietuitor!";
            hint      = "R = Joaca din nou   ESC = Iesire";
            title_col = LIME;
            break;
        case STATE_LOSE:
            title     = "AI MURIT...";
            subtitle  = "Zombie-ii te-au prins.";
            hint      = "R = Incearca din nou   ESC = Iesire";
            title_col = RED;
            break;
        default:
            break;
    }

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    /* Titlu principal */
    int tw = MeasureText(title, 52);
    DrawText(title, cx - tw / 2, cy - 70, 52, title_col);

    /* Subtitlu */
    int sw = MeasureText(subtitle, 24);
    DrawText(subtitle, cx - sw / 2, cy, 24, LIGHTGRAY);

    /* Hint taste */
    int hw = MeasureText(hint, 18);
    DrawText(hint, cx - hw / 2, cy + 40, 18, GRAY);

    /* Indicatie chei (doar la meniu) */
    if (state == STATE_MENU) {
        char keys_info[64];
        snprintf(keys_info, sizeof(keys_info),
                 "Colecteaza %d chei (K) si gaseste EXIT-ul verde", NUM_KEYS);
        int ki = MeasureText(keys_info, 16);
        DrawText(keys_info, cx - ki / 2, cy + 75, 16, DARKGRAY);
    }
}
