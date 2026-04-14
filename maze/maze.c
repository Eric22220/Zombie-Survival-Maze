/* ============================================================
 *  maze/maze.c  –  Generator IMPERFECT cu Recursive Backtracking
 *                  + Carve Extra Passages (bucle/scurtături)
 *
 *  Faza 1: Recursive Backtracking → labirint perfect (un singur drum)
 *  Faza 2: carve_loops()          → sparge ~35% din pereții interiori
 *                                   eligibili pentru a crea bucle,
 *                                   zone deschise și căi alternative
 *  Faza 3: carve_open_rooms()     → sapă 4 camere mici (3x3) în
 *                                   zone aleatoare, dând jucătorului
 *                                   spațiu să manevreze și să evite zombie-ii
 *
 *  Rezultat: labirint cu MULTIPLE drumuri, scurtături și camere —
 *  jucătorul poate evita zombie-ii fugind pe rute alternative.
 * ============================================================ */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "maze/maze.h"

static const int DIRS[4][2] = {{0, -2}, {0, 2}, {-2, 0}, {2, 0}};

static void shuffle4(int order[4]) {
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
    }
}

/* ---- Faza 1: DFS Recursive Backtracking ---- */
static void carve_passages(Maze *m, int cx, int cy) {
    int order[4] = {0, 1, 2, 3};
    shuffle4(order);
    for (int i = 0; i < 4; i++) {
        int dx = DIRS[order[i]][0];
        int dy = DIRS[order[i]][1];
        int nx = cx + dx, ny = cy + dy;
        if (nx > 0 && nx < m->cols - 1 &&
            ny > 0 && ny < m->rows - 1 &&
            m->grid[ny][nx] == TILE_WALL)
        {
            m->grid[cy + dy/2][cx + dx/2] = TILE_FLOOR;
            m->grid[ny][nx]               = TILE_FLOOR;
            carve_passages(m, nx, ny);
        }
    }
}

/* ---- Faza 2: Sparge pereți interiori → bucle și scurtături ----
   Dacă un perete separă 2 celule FLOOR pe axa H sau V,
   îl spargem cu probabilitatea `chance`.
   La 0.35 → ~35% din pereții eligibili dispar = harta respiră.    */
static void carve_loops(Maze *m, float chance) {
    for (int r = 1; r < m->rows - 1; r++) {
        for (int c = 1; c < m->cols - 1; c++) {
            if (m->grid[r][c] != TILE_WALL) continue;

            int horiz = (m->grid[r][c-1] == TILE_FLOOR &&
                         m->grid[r][c+1] == TILE_FLOOR);
            int vert  = (m->grid[r-1][c] == TILE_FLOOR &&
                         m->grid[r+1][c] == TILE_FLOOR);

            if ((horiz || vert) && (float)rand() / RAND_MAX < chance)
                m->grid[r][c] = TILE_FLOOR;
        }
    }
}

/* ---- Faza 3: Camere deschise 3×3 ----
   Oferă jucătorului spații în care poate manevra în jurul zombie-ilor. */
static void carve_open_rooms(Maze *m, int num_rooms, int room_size) {
    for (int n = 0; n < num_rooms; n++) {
        int margin = 2;
        int max_c = m->cols - margin - room_size;
        int max_r = m->rows - margin - room_size;
        if (max_c <= margin || max_r <= margin) continue;

        int rx = margin + rand() % max_c;
        int ry = margin + rand() % max_r;

        for (int dr = 0; dr < room_size; dr++)
            for (int dc = 0; dc < room_size; dc++) {
                int nr = ry + dr, nc = rx + dc;
                if (nr > 0 && nr < m->rows - 1 &&
                    nc > 0 && nc < m->cols - 1)
                    m->grid[nr][nc] = TILE_FLOOR;
            }
    }
}

static int pos_in_keys(const Maze *m, Vec2i p, int placed) {
    for (int i = 0; i < placed; i++)
        if (m->keys[i].x == p.x && m->keys[i].y == p.y) return 1;
    return 0;
}

/* ---- API PUBLIC ---- */

Maze *maze_new(int rows, int cols) {
    Maze *m = (Maze *)malloc(sizeof(Maze));
    m->rows = rows;
    m->cols = cols;
    m->grid = (int **)malloc(rows * sizeof(int *));
    for (int r = 0; r < rows; r++)
        m->grid[r] = (int *)calloc(cols, sizeof(int));
    return m;
}

void maze_generate(Maze *m, unsigned int seed) {
    srand(seed);

    /* Reset */
    for (int r = 0; r < m->rows; r++)
        for (int c = 0; c < m->cols; c++)
            m->grid[r][c] = TILE_WALL;

    /* Faza 1 */
    m->grid[1][1] = TILE_FLOOR;
    carve_passages(m, 1, 1);

    /* Faza 2: 35% din peretii eligibili sparti → multiple rute */
    carve_loops(m, 0.35f);

    /* Faza 3: 4 camere 3x3 → zone de respiro */
    carve_open_rooms(m, 4, 3);

    /* Start si Exit */
    m->start.x = 1;           m->start.y = 1;
    m->exit.x  = m->cols - 2; m->exit.y  = m->rows - 2;
    m->grid[m->start.y][m->start.x] = TILE_FLOOR;
    m->grid[m->exit.y ][m->exit.x ] = TILE_FLOOR;

    /* Plasam cheile la distanta minima de start (1/3 din harta)
       pe celule FLOOR aleatoare → nu mai sunt mereu pe acelasi drum */
    int placed = 0, attempts = 0;
    while (placed < NUM_KEYS && attempts < 20000) {
        attempts++;
        int kx = 1 + rand() % (m->cols - 2);
        int ky = 1 + rand() % (m->rows - 2);

        if (m->grid[ky][kx] != TILE_FLOOR) continue;

        int min_dist = (m->cols + m->rows) / 3;
        int dist = abs(kx - m->start.x) + abs(ky - m->start.y);
        if (dist < min_dist) continue;
        if (kx == m->start.x && ky == m->start.y) continue;
        if (kx == m->exit.x  && ky == m->exit.y)  continue;
        if (pos_in_keys(m, (Vec2i){kx, ky}, placed)) continue;

        m->keys[placed].x = kx;
        m->keys[placed].y = ky;
        placed++;
    }

    /* Fallback */
    for (int i = placed; i < NUM_KEYS; i++)
        m->keys[i] = m->exit;
}

void maze_free(Maze *m) {
    for (int r = 0; r < m->rows; r++) free(m->grid[r]);
    free(m->grid);
    free(m);
}

int maze_in_bounds(const Maze *m, int x, int y) {
    return (x >= 0 && x < m->cols && y >= 0 && y < m->rows);
}

int maze_is_floor(const Maze *m, int x, int y) {
    return maze_in_bounds(m, x, y) && (m->grid[y][x] == TILE_FLOOR);
}
