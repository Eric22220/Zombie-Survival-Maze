/* ============================================================
 *  brain/pathfinder.c  –  BFS (Breadth-First Search) pe matrice
 *
 *  Cum functioneaza:
 *  1. Pornim din pozitia zombie-ului ('from')
 *  2. Exploram vecinii in strat cu strat (via Coada)
 *  3. Tinem un array 'parent[]' pentru a reconstrui drumul
 *  4. Cand atingem jucatorul ('to'), mergem inapoi prin parent
 *     pana ajungem la primul pas de langa 'from'
 *  5. Acel prim pas = directia in care se misca zombie-ul
 * ============================================================ */

#include <stdlib.h>
#include <string.h>
#include "brain/pathfinder.h"
#include "brain/queue.h"

/* Macro pentru indexarea flat a matricei 2D */
#define IDX(x, y)  ((y) * m->cols + (x))

/* Cele 4 directii cardinale (stanga, dreapta, sus, jos) */
static const int DX[4] = {-1,  1,  0,  0};
static const int DY[4] = { 0,  0, -1,  1};

Vec2i bfs_next_step(const Maze *m, Vec2i from, Vec2i to) {
    /* Cazul trivial: suntem deja la destinatie */
    if (from.x == to.x && from.y == to.y) return from;

    int total = m->rows * m->cols;

    /* Array-ul de parinti: parent[idx] = celula din care am ajuns la idx */
    Vec2i *parent  = (Vec2i *)malloc(total * sizeof(Vec2i));
    int   *visited = (int   *)calloc(total,  sizeof(int));

    /* Initializam toti parintii ca invalidi */
    for (int i = 0; i < total; i++) {
        parent[i] = (Vec2i){-1, -1};
    }

    Queue *q = queue_new();
    queue_push(q, from);
    visited[IDX(from.x, from.y)] = 1;

    int found = 0;

    /* ---- BFS principal ---- */
    while (!queue_empty(q) && !found) {
        Vec2i cur = queue_pop(q);

        for (int d = 0; d < 4; d++) {
            int nx = cur.x + DX[d];
            int ny = cur.y + DY[d];

            if (!maze_in_bounds(m, nx, ny))          continue;
            if (m->grid[ny][nx] != TILE_FLOOR)        continue;
            if (visited[IDX(nx, ny)])                 continue;

            visited[IDX(nx, ny)]  = 1;
            parent [IDX(nx, ny)]  = cur;

            if (nx == to.x && ny == to.y) {
                found = 1;
                break;
            }
            queue_push(q, (Vec2i){nx, ny});
        }
    }

    queue_free(q);

    /* ---- Reconstructia drumului ---- */
    Vec2i result = from; /* implicit: nu te misca */

    if (found) {
        /* Mergem inapoi de la 'to' spre 'from' prin parent[] */
        Vec2i cur  = to;
        int safety = total; /* protectie impotriva buclelor infinite */

        while (safety-- > 0) {
            Vec2i par = parent[IDX(cur.x, cur.y)];
            /* Daca parintele lui cur este chiar 'from',
               atunci 'cur' este primul pas pe drum */
            if (par.x == from.x && par.y == from.y) {
                result = cur;
                break;
            }
            /* Parintele invalid = ceva a mers prost */
            if (par.x == -1) {
                result = from;
                break;
            }
            cur = par;
        }
    }

    free(parent);
    free(visited);
    return result;
}

#undef IDX
