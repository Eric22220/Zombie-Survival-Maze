#ifndef ZOMBIE_H
#define ZOMBIE_H

/* ============================================================
 *  world/zombie.h  –  Inamicul controlat de AI (BFS)
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"

typedef struct {
    Vec2i pos;          /* pozitia curenta in grila     */
    int   alive;        /* 1 daca e activ               */
    float move_timer;   /* acumulator de timp (secunde) */
} Zombie;

/* Initializeaza un zombie la pozitia data */
void zombie_init(Zombie *z, Vec2i pos);

/* Actualizeaza pozitia zombie-ului:
   la fiecare ZOMBIE_SPEED secunde, apeleaza BFS si face un pas
   spre pozitia jucatorului (player_pos). */
void zombie_update(Zombie *z, const Maze *m, Vec2i player_pos, float dt);

#endif /* ZOMBIE_H */
