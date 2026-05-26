/* ============================================================
 *  world/level.c  –  Tabel cu config-uri per nivel
 *
 *  Curba de dificultate (tweaked manual pentru balans):
 *
 *   Nv | Maze  | Zombii | Z-speed | Chei | Powerups | Loops
 *   ---|-------|--------|---------|------|----------|------
 *    1 | 21x21 |   2    |  0.55s  |  3   |   3      |  25
 *    2 | 21x21 |   3    |  0.52s  |  3   |   3      |  27
 *    3 | 23x23 |   3    |  0.50s  |  3   |   3      |  31
 *    4 | 23x23 |   4    |  0.47s  |  4   |   3      |  33
 *    5 | 25x25 |   4    |  0.44s  |  4   |   3      |  37
 *    6 | 25x25 |   5    |  0.42s  |  4   |   3      |  39
 *    7 | 27x27 |   5    |  0.40s  |  5   |   3      |  43
 *    8 | 27x27 |   6    |  0.37s  |  5   |   3      |  45
 *    9 | 29x29 |   7    |  0.34s  |  5   |   3      |  51
 *   10 | 31x31 |   8    |  0.30s  |  6   |   3      |  57
 *
 *  Loops = pereti sparti dupa Recursive Backtracking. Astfel
 *  labirintul nu mai e "perfect" (drum unic intre orice 2 puncte)
 *  ci are multiple drumuri → ai pe unde fugi de zombi.
 *
 *  Scalare: ~num_zombies * 5 + dim/2. Mai multi zombi
 *  necesita mai multe optiuni de fuga.
 * ============================================================ */

#include "world/level.h"

static const LevelConfig LEVELS[MAX_LEVELS] = {
    /* level, dim, zomb, zspd, keys, pups, loops */
    {  1,  21,  2, 0.55f, 3, 3, 25 },
    {  2,  21,  3, 0.52f, 3, 3, 27 },
    {  3,  23,  3, 0.50f, 3, 3, 31 },
    {  4,  23,  4, 0.47f, 4, 3, 33 },
    {  5,  25,  4, 0.44f, 4, 3, 37 },
    {  6,  25,  5, 0.42f, 4, 3, 39 },
    {  7,  27,  5, 0.40f, 5, 3, 43 },
    {  8,  27,  6, 0.37f, 5, 3, 45 },
    {  9,  29,  7, 0.34f, 5, 3, 51 },
    { 10,  31,  8, 0.30f, 6, 3, 57 }
};

LevelConfig level_get_config(int level) {
    if (level < 1)            level = 1;
    if (level > MAX_LEVELS)   level = MAX_LEVELS;
    return LEVELS[level - 1];
}
