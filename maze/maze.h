#ifndef MAZE_H
#define MAZE_H

/* ============================================================
 *  maze/maze.h  –  Structura labirintului generat procedural
 *
 *  Spre deosebire de versiunea initiala, numarul de chei si
 *  dimensiunea labirintului sunt acum variabile per nivel.
 *  keys[] are dimensiune fixa MAX_KEYS — campul num_keys spune
 *  cate sunt efectiv folosite (primele num_keys sloturi).
 * ============================================================ */

#include "core.h"

typedef struct {
    int   rows;
    int   cols;
    int **grid;                 /* 0 = WALL, 1 = FLOOR (alocat cu malloc) */
    Vec2i start;                /* pozitia de start a jucatorului          */
    Vec2i exit;                 /* pozitia iesirii                         */
    int   num_keys;             /* cate chei sunt active (pana la MAX_KEYS)*/
    Vec2i keys[MAX_KEYS];       /* pozitiile cheilor (primele num_keys)    */
} Maze;

/* Aloca un labirint gol (toate peretii) cu dimensiunile date.
   rows si cols TREBUIE sa fie impare. */
Maze *maze_new(int rows, int cols);

/* Genereaza labirintul cu Recursive Backtracking si plaseaza num_keys chei.
   seed = numar pentru srand(), schimba layout-ul la fiecare apel. */
void  maze_generate(Maze *m, unsigned int seed, int num_keys);

/* Sparge num_loops pereti "interiori" (cu floor pe parti opuse).
   Transforma labirintul perfect (drum unic) intr-unul cu multiple drumuri,
   astfel incat jucatorul poate fugi de zombi pe rute alternative.
   Apeleaza-l DUPA maze_generate. */
void  maze_add_loops(Maze *m, int num_loops);

/* Elibereaza toata memoria alocata */
void  maze_free(Maze *m);

/* Verifica daca (x,y) e in limite si e celula FLOOR */
int   maze_is_floor(const Maze *m, int x, int y);

/* Verifica daca (x,y) e in limitele grilei */
int   maze_in_bounds(const Maze *m, int x, int y);

#endif /* MAZE_H */
