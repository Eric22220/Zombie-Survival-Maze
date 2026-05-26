/* ============================================================
 *  world/powerup.c  –  Spawn random + utilitare power-ups
 * ============================================================ */

#include <stdlib.h>
#include "world/powerup.h"

const char *powerup_name(PowerupType t) {
    switch (t) {
        case POWERUP_SPEED:      return "VITEZA";
        case POWERUP_FREEZE:     return "INGHET";
        case POWERUP_HEAL:       return "VIATA";
        case POWERUP_INVINCIBLE: return "INVINCIBIL";
        case POWERUP_KILL_ALL:   return "PURGE";
        default:                 return "?";
    }
}

const char *powerup_glyph(PowerupType t) {
    switch (t) {
        case POWERUP_SPEED:      return "S";
        case POWERUP_FREEZE:     return "F";
        case POWERUP_HEAL:       return "+";
        case POWERUP_INVINCIBLE: return "I";
        case POWERUP_KILL_ALL:   return "X";
        default:                 return "?";
    }
}

void powerups_spawn(Powerup *pus, int count, const Maze *m,
                    Vec2i start, Vec2i exit_pos)
{
    /* Initializeaza ca "morti" pentru sloturile neutilizate */
    for (int i = 0; i < MAX_POWERUPS; i++) {
        pus[i].alive = 0;
    }

    int placed   = 0;
    int attempts = 0;
    while (placed < count && attempts < 10000) {
        attempts++;

        /* Celulele "reale" ale labirintului sunt la pozitii impare */
        int kx = 1 + (rand() % ((m->cols - 1) / 2)) * 2;
        int ky = 1 + (rand() % ((m->rows - 1) / 2)) * 2;

        if (m->grid[ky][kx] != TILE_FLOOR)            continue;
        if (kx == start.x    && ky == start.y)        continue;
        if (kx == exit_pos.x && ky == exit_pos.y)     continue;

        /* Evitam suprapunere cu alt powerup */
        int dup = 0;
        for (int i = 0; i < placed; i++) {
            if (pus[i].pos.x == kx && pus[i].pos.y == ky) {
                dup = 1;
                break;
            }
        }
        if (dup) continue;

        /* Evitam suprapunere cu chei */
        for (int i = 0; i < m->num_keys; i++) {
            if (m->keys[i].x == kx && m->keys[i].y == ky) {
                dup = 1;
                break;
            }
        }
        if (dup) continue;

        pus[placed].pos.x = kx;
        pus[placed].pos.y = ky;
        pus[placed].type  = (PowerupType)(rand() % NUM_POWERUP_TYPES);
        pus[placed].alive = 1;
        placed++;
    }
}
