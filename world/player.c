/* ============================================================
 *  world/player.c  –  Input + miscare cu auto-repeat
 *
 *  Logica de input:
 *   1. Daca o tasta a fost APASATA in acest frame (IsKeyPressed)
 *      → un pas imediat + reset hold_timer la HOLD_INITIAL_DELAY
 *   2. Altfel, daca o tasta e TINUTA apasata (IsKeyDown)
 *      → decrement hold_timer; cand ajunge la 0, un pas si
 *        reset la HOLD_REPEAT_INTERVAL (sau _FAST cu speed boost)
 *   3. Daca nu e nimic apasat → reset hold_timer
 *
 *  Astfel:
 *   - Tap rapid (spam) → cea mai rapida miscare posibila (1 pas/frame)
 *   - Hold static     → ritm constant, controlabil
 *   - Speed power-up  → ritmul de hold devine mult mai rapid
 * ============================================================ */

#include <raylib.h>
#include "world/player.h"

Player player_new(Vec2i start) {
    Player p;
    p.pos                = start;
    p.health             = PLAYER_HEALTH;
    p.keys_collected     = 0;
    p.invincible_timer   = 0;
    p.speed_timer        = 0.0f;
    p.invincible_pu_timer= 0.0f;
    p.hold_timer         = 0.0f;
    return p;
}

/* Helper: incearca sa miste in directia (dx, dy). Returneaza 1 daca a reusit. */
static int try_move(Player *p, const Maze *m, int dx, int dy) {
    Vec2i next = { p->pos.x + dx, p->pos.y + dy };
    if (maze_is_floor(m, next.x, next.y)) {
        p->pos = next;
        return 1;
    }
    return 0;
}

/* Determina (dx, dy) pe baza tastelor. Daca pressed_only=1, foloseste
   IsKeyPressed (instant); altfel IsKeyDown (continuu).
   Returneaza 1 daca o directie e activa.
   Prioritate: sus → jos → stanga → dreapta. */
static int get_direction(int pressed_only, int *out_dx, int *out_dy) {
    *out_dx = 0;
    *out_dy = 0;

    if (pressed_only) {
        if      (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    { *out_dy = -1; return 1; }
        else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  { *out_dy =  1; return 1; }
        else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  { *out_dx = -1; return 1; }
        else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { *out_dx =  1; return 1; }
    } else {
        if      (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    { *out_dy = -1; return 1; }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  { *out_dy =  1; return 1; }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  { *out_dx = -1; return 1; }
        else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { *out_dx =  1; return 1; }
    }
    return 0;
}

void player_update(Player *p, const Maze *m, float dt) {
    /* ---- Decrement timer-e ---- */
    if (p->invincible_timer > 0)        p->invincible_timer--;
    if (p->speed_timer        > 0.0f) { p->speed_timer        -= dt; if (p->speed_timer        < 0) p->speed_timer        = 0; }
    if (p->invincible_pu_timer> 0.0f) { p->invincible_pu_timer -= dt; if (p->invincible_pu_timer< 0) p->invincible_pu_timer = 0; }

    int dx, dy;

    /* ---- 1. Verifica press nou (instant response) ---- */
    if (get_direction(1 /*pressed*/, &dx, &dy)) {
        try_move(p, m, dx, dy);
        /* Dupa un press fresh, asteptam HOLD_INITIAL_DELAY inainte sa repete */
        p->hold_timer = HOLD_INITIAL_DELAY;
        return;
    }

    /* ---- 2. Daca nu e press nou, verifica hold (auto-repeat) ---- */
    if (get_direction(0 /*down*/, &dx, &dy)) {
        p->hold_timer -= dt;
        if (p->hold_timer <= 0.0f) {
            try_move(p, m, dx, dy);
            /* Reset timer: mai rapid daca avem power-up SPEED */
            float interval = (p->speed_timer > 0.0f)
                             ? HOLD_REPEAT_INTERVAL_FAST
                             : HOLD_REPEAT_INTERVAL;
            p->hold_timer = interval;
        }
    } else {
        /* Nimic apasat → reset, pentru ca urmatorul press sa fie instant */
        p->hold_timer = 0.0f;
    }
}

void player_collect_key(Player *p) {
    p->keys_collected++;
}

int player_is_alive(const Player *p) {
    return (p->health > 0);
}

int player_is_invincible(const Player *p) {
    return (p->invincible_timer > 0) || (p->invincible_pu_timer > 0.0f);
}
