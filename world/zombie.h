#ifndef ZOMBIE_H
#define ZOMBIE_H

/* ============================================================
 *  world/zombie.h  –  Inamicul controlat de AI (BFS)
 *
 *  Schimbari fata de versiunea originala:
 *    - viteza nu mai e #define ZOMBIE_SPEED, vine ca parametru
 *      din LevelConfig
 *    - zombie_update primeste si un flag 'frozen' (de la power-up)
 *    - retinem pozitia de spawn ca sa stim unde sa-l reinviem
 *      dupa KILL_ALL
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"

typedef struct {
    Vec2i pos;          /* pozitia curenta in grila        */
    Vec2i spawn;        /* pozitia originala (pt. reinviere) */
    int   alive;        /* 1 daca e activ                  */
    float move_timer;   /* acumulator de timp (secunde)    */
} Zombie;

/* Initializeaza un zombie la pozitia data (e si pozitia de spawn) */
void zombie_init(Zombie *z, Vec2i pos);

/* Update logica de miscare:
   - dt          = secunde scurse de la ultimul frame
   - zombie_speed= secunde intre mutari (din LevelConfig)
   - frozen      = daca 1, zombie nu se misca (power-up FREEZE) */
void zombie_update(Zombie *z, const Maze *m, Vec2i player_pos,
                   float dt, float zombie_speed, int frozen);

#endif /* ZOMBIE_H */
