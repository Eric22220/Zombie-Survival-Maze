#ifndef MAZE_H
#define MAZE_H

/* ============================================================
 *  maze/maze.h  –  Structura labirintului generat procedural
 * ============================================================ */

#include "core.h"

typedef struct {
    int   rows;
    int   cols;
    int **grid;             /* 0 = WALL, 1 = FLOOR  (alocat cu malloc) */
    Vec2i start;            /* pozitia de start a jucatorului           */
    Vec2i exit;             /* pozitia iesirii                          */
    Vec2i keys[NUM_KEYS];   /* pozitiile cheilor de colectat            */
} Maze;

/* Aloca un labirint gol (toate peretii) */
Maze *maze_new(int rows, int cols);

/* Genereaza labirintul cu Recursive Backtracking (DFS)
   seed = numar pentru srand(), schimba labirintul la fiecare pornire */
void  maze_generate(Maze *m, unsigned int seed);

/* Elibereaza toata memoria alocata */
void  maze_free(Maze *m);

/* Verifica daca (x,y) e in limite si e celula FLOOR */
int   maze_is_floor(const Maze *m, int x, int y);

/* Verifica daca (x,y) e in limitele grilei */
int   maze_in_bounds(const Maze *m, int x, int y);

#endif /* MAZE_H */
