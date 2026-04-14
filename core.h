#ifndef CORE_H
#define CORE_H

/* ============================================================
 *  core.h  –  Constante globale, tipuri de baza, stari de joc
 * ============================================================ */

/* --- Dimensiunile labirintului (TREBUIE sa fie numere impare) --- */
#define MAZE_COLS 21
#define MAZE_ROWS 21

/* --- Afisare --- */
#define TILE_SIZE   32          /* pixeli per celula                */
#define HUD_HEIGHT  60          /* bara de stare de jos             */
#define SCREEN_W    (MAZE_COLS * TILE_SIZE)
#define SCREEN_H    (MAZE_ROWS * TILE_SIZE + HUD_HEIGHT)
#define FPS         60

/* --- Gameplay --- */
#define NUM_ZOMBIES     4
#define NUM_KEYS        3
#define ZOMBIE_SPEED    0.45f   /* secunde intre mutarile zombie    */
#define PLAYER_HEALTH   3
#define INVINCIBLE_FRAMES 90    /* ~1.5s la 60FPS dupa lovitura     */

/* --- Tipuri de celule in grila labirintului --- */
typedef enum {
    TILE_WALL  = 0,
    TILE_FLOOR = 1
} TileType;

/* --- Vector 2D de intregi (coloana x, rand y) --- */
typedef struct {
    int x;  /* coloana */
    int y;  /* rand    */
} Vec2i;

/* --- Starile principale ale jocului --- */
typedef enum {
    STATE_MENU    = 0,
    STATE_PLAYING = 1,
    STATE_WIN     = 2,
    STATE_LOSE    = 3
} GameState;

#endif /* CORE_H */
