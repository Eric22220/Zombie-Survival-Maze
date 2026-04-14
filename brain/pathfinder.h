#ifndef PATHFINDER_H
#define PATHFINDER_H

/* ============================================================
 *  brain/pathfinder.h  –  AI de pathfinding cu BFS
 *
 *  BFS (Breadth-First Search) pe matrice garanteaza drumul
 *  cel mai SCURT intre zombie si jucator, ocolind peretii.
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"

/* Returneaza URMATORUL pas pe care trebuie sa-l faca entitatea
 * aflata in 'from' pentru a ajunge la 'to' pe drumul cel mai scurt.
 * Daca nu exista drum, returneaza 'from' (stai pe loc).
 *
 * Complexitate: O(rows * cols)  per apel.
 */
Vec2i bfs_next_step(const Maze *m, Vec2i from, Vec2i to);

#endif /* PATHFINDER_H */
