#ifndef PLAYER_H
#define PLAYER_H

/* ============================================================
 *  world/player.h  –  Jucatorul controlat de tastatura
 *
 *  Sistem unificat de input cu auto-repeat:
 *    - Apesi scurt   → 1 pas instant (cea mai rapida reactie posibila)
 *    - Tii apasat    → dupa HOLD_INITIAL_DELAY, repeta pasul
 *                      la fiecare HOLD_REPEAT_INTERVAL
 *    - Spamezi       → fiecare apasare = 1 pas instant (foarte rapid)
 *
 *  Cand ai power-up SPEED activ, intervalul de repeat e mai mic
 *  (HOLD_REPEAT_INTERVAL_FAST), deci tinutul apasat e mult mai rapid.
 *
 *  invincible_timer (FRAMES) e separat – e pt. invincibilitatea
 *  post-hit, NU de la power-up.
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"

typedef struct {
    Vec2i pos;                      /* pozitia curenta in grila         */
    int   health;                   /* puncte de viata ramase           */
    int   keys_collected;           /* cate chei a adunat pana acum     */
    int   invincible_timer;         /* cadre post-hit (frames)          */

    /* Power-up effect timers (secunde, count down to 0) */
    float speed_timer;              /* > 0  → mod SPEED activ            */
    float invincible_pu_timer;      /* > 0  → imun la zombi              */

    /* Internal pentru auto-repeat la hold */
    float hold_timer;               /* timpul ramas pana la urmatorul pas auto */
} Player;

/* Initializeaza jucatorul la pozitia de start */
Player player_new(Vec2i start);

/* Proceseaza input + decrementeaza timer-uri (dt = secunde / frame) */
void   player_update(Player *p, const Maze *m, float dt);

/* Incrementeaza numarul de chei colectate */
void   player_collect_key(Player *p);

/* Returneaza 1 daca jucatorul e inca in viata */
int    player_is_alive(const Player *p);

/* 1 daca jucatorul nu poate primi damage (din orice motiv) */
int    player_is_invincible(const Player *p);

#endif /* PLAYER_H */
