#ifndef LEVEL_H
#define LEVEL_H

/* ============================================================
 *  world/level.h  –  Configuratia de dificultate per nivel
 *
 *  Curba e tweaked manual (NU liniara) ca sa fie balansata:
 *  nivelul 1 = usor introductiv, nivelul 10 = haos controlat.
 * ============================================================ */

#include "core.h"

typedef struct {
    int   level;            /* 1..MAX_LEVELS                              */
    int   maze_dim;         /* dimensiune patrata (21-31, mereu impara)   */
    int   num_zombies;      /* 2..8                                       */
    float zombie_speed;     /* secunde intre mutari (mai mic = mai rapid) */
    int   num_keys;         /* 3..6                                       */
    int   num_powerups;     /* power-uri spawn-uite per nivel             */
    int   num_loops;        /* pereti sparti dupa generare = mai multe drumuri */
} LevelConfig;

/* Returneaza config-ul pentru nivelul dat (clamped la [1, MAX_LEVELS]) */
LevelConfig level_get_config(int level);

#endif /* LEVEL_H */
