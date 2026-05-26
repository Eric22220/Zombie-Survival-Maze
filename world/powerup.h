#ifndef POWERUP_H
#define POWERUP_H

/* ============================================================
 *  world/powerup.h  –  Power-uri random pe jos in labirint
 *
 *  Cele 5 tipuri (vezi enum PowerupType din core.h):
 *    SPEED      – player se misca rapid cu IsKeyDown
 *    FREEZE     – zombii sunt inghetati pe loc
 *    HEAL       – +1 viata (cap la PLAYER_HEALTH)
 *    INVINCIBLE – zombii nu mai pot lovi
 *    KILL_ALL   – toti zombii "mor" temporar, apoi revin
 *
 *  Toate au durata POWERUP_DURATION (din core.h),
 *  in afara de HEAL care e instant.
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"

typedef struct {
    Vec2i       pos;
    PowerupType type;
    int         alive;    /* 1 = inca pe jos, 0 = colectat */
} Powerup;

/* Spawn 'count' power-uri random pe celule FLOOR libere
   (evita start, exit, chei si alte power-uri deja plasate).
   Tipul fiecaruia e ales uniform random dintre cele 5. */
void powerups_spawn(Powerup *pus, int count, const Maze *m,
                    Vec2i start, Vec2i exit_pos);

/* Numele power-up-ului in romana (pentru HUD) */
const char *powerup_name(PowerupType t);

/* Glifa de o litera afisata pe celula */
const char *powerup_glyph(PowerupType t);

#endif /* POWERUP_H */
