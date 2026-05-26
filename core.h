#ifndef CORE_H
#define CORE_H

/* ============================================================
 *  core.h  –  Constante globale, tipuri de baza, stari de joc
 * ============================================================ */

/* --- Dimensiunile labirintului ---
 * Toate dimensiunile sunt patrate si IMPARE.
 * MIN = nivelul 1, MAX = nivelul 10.
 */
#define MIN_MAZE_DIM    21
#define MAX_MAZE_DIM    31

/* --- Afisare --- */
#define TILE_SIZE       28
#define HUD_HEIGHT      80
#define SCREEN_W        (MAX_MAZE_DIM * TILE_SIZE)
#define SCREEN_H        (MAX_MAZE_DIM * TILE_SIZE + HUD_HEIGHT)
#define FPS             60

/* --- Sistem de nivele --- */
#define MAX_LEVELS      10

/* --- Limite max (alocare statica) --- */
#define MAX_ZOMBIES     12
#define MAX_KEYS        8
#define MAX_POWERUPS    8

/* --- Gameplay --- */
#define PLAYER_HEALTH       3
#define INVINCIBLE_FRAMES   90      /* ~1.5s la 60FPS dupa lovitura */

/* --- Power-ups --- */
#define POWERUP_DURATION        4.0f    /* secunde cat tine un efect */
#define PLAYER_SPEED_INTERVAL   0.10f   /* secunde intre pasi in mod SPEED */

/* --- Auto-repeat la miscare (cand tii apasata o tasta) --- */
#define HOLD_INITIAL_DELAY        0.25f   /* delay dupa primul press inainte sa inceapa repeat-ul */
#define HOLD_REPEAT_INTERVAL      0.12f   /* interval intre pasi cand tii apasat (normal) */
#define HOLD_REPEAT_INTERVAL_FAST 0.06f   /* interval cand ai power-up SPEED */

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
    STATE_MENU           = 0,   /* ecran de start cu titlu             */
    STATE_LEVEL_SELECT   = 1,   /* lobby cu nivelele 1-10               */
    STATE_PLAYING        = 2,   /* jocul propriu-zis                    */
    STATE_LEVEL_COMPLETE = 3,   /* "Nivel completat!"                   */
    STATE_LOSE           = 4,   /* "Ai murit"                           */
    STATE_GAME_COMPLETE  = 5    /* "Felicitari, ai terminat tot!"       */
} GameState;

/* --- Tipuri de power-ups --- */
typedef enum {
    POWERUP_SPEED      = 0,   /* miscare rapida cu IsKeyDown            */
    POWERUP_FREEZE     = 1,   /* zombii inghetati                       */
    POWERUP_HEAL       = 2,   /* +1 viata (cap la PLAYER_HEALTH)        */
    POWERUP_INVINCIBLE = 3,   /* zombii nu te lovesc                    */
    POWERUP_KILL_ALL   = 4,   /* toti zombii dispar temporar            */
    NUM_POWERUP_TYPES  = 5
} PowerupType;

#endif /* CORE_H */
