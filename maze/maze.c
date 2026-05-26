/* ============================================================
 *  maze/maze.c  –  Generare procedurala (Recursive Backtracking)
 *
 *  Identic ca logica cu versiunea originala. Doua schimbari:
 *    1. maze_generate primeste num_keys (in loc de NUM_KEYS hardcodat)
 *    2. m->num_keys retine cate chei am plasat efectiv
 * ============================================================ */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "maze/maze.h"

/* Cele 4 directii (saptam cu 2 celule ca sa lasam perete intre coridoare) */
static const int DIRS[4][2] = {{0, -2}, {0, 2}, {-2, 0}, {2, 0}};

/* Fisher-Yates shuffle pe array-ul de 4 directii */
static void shuffle(int order[4]) {
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp   = order[i];
        order[i]  = order[j];
        order[j]  = tmp;
    }
}

/* Recursie principala – sapa coridoare din (cx, cy) */
static void carve_passages(Maze *m, int cx, int cy) {
    int order[4] = {0, 1, 2, 3};
    shuffle(order);

    for (int i = 0; i < 4; i++) {
        int dx = DIRS[order[i]][0];
        int dy = DIRS[order[i]][1];
        int nx = cx + dx;
        int ny = cy + dy;

        if (nx > 0 && nx < m->cols - 1 &&
            ny > 0 && ny < m->rows - 1 &&
            m->grid[ny][nx] == TILE_WALL)
        {
            m->grid[cy + dy / 2][cx + dx / 2] = TILE_FLOOR;
            m->grid[ny][nx]                    = TILE_FLOOR;
            carve_passages(m, nx, ny);
        }
    }
}

/* ------------------------------------------------------------ */

Maze *maze_new(int rows, int cols) {
    Maze *m   = (Maze *)malloc(sizeof(Maze));
    m->rows   = rows;
    m->cols   = cols;
    m->num_keys = 0;
    m->grid   = (int **)malloc(rows * sizeof(int *));
    for (int r = 0; r < rows; r++) {
        m->grid[r] = (int *)calloc(cols, sizeof(int));
    }
    return m;
}

void maze_generate(Maze *m, unsigned int seed, int num_keys) {
    srand(seed);

    /* Clamp num_keys la [1, MAX_KEYS] */
    if (num_keys < 1)        num_keys = 1;
    if (num_keys > MAX_KEYS) num_keys = MAX_KEYS;

    /* Reset – tot perete */
    for (int r = 0; r < m->rows; r++)
        for (int c = 0; c < m->cols; c++)
            m->grid[r][c] = TILE_WALL;

    /* Pornim din coltul stanga-sus */
    m->grid[1][1] = TILE_FLOOR;
    carve_passages(m, 1, 1);

    /* Start si exit fixe in colturi */
    m->start.x = 1;            m->start.y = 1;
    m->exit.x  = m->cols - 2;  m->exit.y  = m->rows - 2;

    /* Plasam cheile pe celule FLOOR random (evitand start/exit) */
    int placed   = 0;
    int attempts = 0;
    while (placed < num_keys && attempts < 10000) {
        attempts++;
        int kx = 1 + (rand() % ((m->cols - 1) / 2)) * 2;
        int ky = 1 + (rand() % ((m->rows - 1) / 2)) * 2;

        if (m->grid[ky][kx] != TILE_FLOOR)            continue;
        if (kx == m->start.x && ky == m->start.y)     continue;
        if (kx == m->exit.x  && ky == m->exit.y)      continue;

        int duplicate = 0;
        for (int i = 0; i < placed; i++) {
            if (m->keys[i].x == kx && m->keys[i].y == ky) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {
            m->keys[placed].x = kx;
            m->keys[placed].y = ky;
            placed++;
        }
    }

    m->num_keys = placed;
}

void maze_free(Maze *m) {
    if (m == NULL) return;
    for (int r = 0; r < m->rows; r++) {
        free(m->grid[r]);
    }
    free(m->grid);
    free(m);
}

/* ------------------------------------------------------------ *
 *  maze_add_loops – sparge pereti pentru a crea drumuri multiple
 *
 *  Cautam pereti "interiori" care au floor pe parti OPUSE
 *  (orizontal: floor nord+sud, perete est+vest; sau invers).
 *  Spargandu-i, conectam doua coridoare care erau separate de
 *  un singur perete -> creeam un loop in grafrul de drumuri.
 *
 *  Nu spargem pereti la fel cu intersectii (floor pe ambele directii)
 *  pentru ca acolo nu se formeaza un loop nou.
 * ------------------------------------------------------------ */
void maze_add_loops(Maze *m, int num_loops) {
    if (num_loops <= 0) return;

    int placed   = 0;
    int attempts = 0;
    int max_attempts = num_loops * 50;  /* plafon ca sa nu cicleze degeaba */

    while (placed < num_loops && attempts < max_attempts) {
        attempts++;

        /* Alegem un perete random in interiorul grilei (nu pe bordura) */
        int x = 1 + rand() % (m->cols - 2);
        int y = 1 + rand() % (m->rows - 2);

        if (m->grid[y][x] != TILE_WALL) continue;

        int n = m->grid[y - 1][x];     /* nord */
        int s = m->grid[y + 1][x];     /* sud  */
        int w = m->grid[y][x - 1];     /* vest */
        int e = m->grid[y][x + 1];     /* est  */

        /* Perete vertical care separa 2 coridoare orizontale:
             N=floor, S=floor, W=wall, E=wall  → spargere face loop */
        int h_break = (n == TILE_FLOOR && s == TILE_FLOOR &&
                       w == TILE_WALL  && e == TILE_WALL);

        /* Perete orizontal care separa 2 coridoare verticale:
             N=wall, S=wall, W=floor, E=floor */
        int v_break = (n == TILE_WALL  && s == TILE_WALL  &&
                       w == TILE_FLOOR && e == TILE_FLOOR);

        if (h_break || v_break) {
            m->grid[y][x] = TILE_FLOOR;
            placed++;
        }
    }
}

int maze_in_bounds(const Maze *m, int x, int y) {
    return (x >= 0 && x < m->cols && y >= 0 && y < m->rows);
}

int maze_is_floor(const Maze *m, int x, int y) {
    return maze_in_bounds(m, x, y) && (m->grid[y][x] == TILE_FLOOR);
}
