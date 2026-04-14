#ifndef PLAYER_H
#define PLAYER_H

/* ============================================================
 *  world/player.h  –  Jucatorul controlat de tastatura
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"

typedef struct {
    Vec2i pos;                  /* pozitia curenta in grila         */
    int   health;               /* puncte de viata ramase           */
    int   keys_collected;       /* cate chei a adunat pana acum     */
    int   invincible_timer;     /* cadre de invincibilitate dupa hit */
} Player;

/* Initializeaza jucatorul la pozitia de start */
Player player_new(Vec2i start);

/* Proceseaza inputul de tastatura si muta jucatorul (WASD/Sageti)
   Miscarea e blocata de pereti via maze_is_floor() */
void   player_update(Player *p, const Maze *m);

/* Incrementeaza numarul de chei colectate */
void   player_collect_key(Player *p);

/* Returneaza 1 daca jucatorul e inca in viata */
int    player_is_alive(const Player *p);

#endif /* PLAYER_H */
