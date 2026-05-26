#ifndef RENDERER_H
#define RENDERER_H

/* ============================================================
 *  gfx/renderer.h  –  Declaratii pentru toate functiile de desen
 *
 *  Fereastra are dimensiunea MAX (31x31 * TILE_SIZE + HUD_HEIGHT).
 *  Labirintele mai mici sunt CENTRATE in fereastra cu offset.
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"
#include "world/player.h"
#include "world/zombie.h"
#include "world/powerup.h"
#include "world/level.h"
#include "save.h"

/* --- Offset pentru centrare orizontala/verticala a labirintului --- */
int  maze_offset_x(const Maze *m);
int  maze_offset_y(const Maze *m);

/* --- Randere de scena ---
 * Toate folosesc m + offsets pentru a desena la pozitia corecta. */

void render_maze    (const Maze *m, const int keys_alive[MAX_KEYS]);
void render_exit    (const Maze *m, Vec2i exit_pos, int unlocked);
void render_player  (const Maze *m, const Player *p);
void render_zombies (const Maze *m, const Zombie *zombies, int count);
void render_powerups(const Maze *m, const Powerup *pus, int count);

/* HUD-ul de jos (HP, chei, nivel, power-uri active cu timer) */
void render_hud(const Player *p, const LevelConfig *cfg,
                float freeze_timer, float kill_all_timer);

/* --- Ecrane full-screen --- */

/* Meniul de start (STATE_MENU) */
void render_menu(void);

/* Lobby cu nivelele 1-10 (STATE_LEVEL_SELECT)
   cursor = nivelul evidentiat curent (1..MAX_LEVELS) */
void render_level_select(const SaveData *s, int cursor);

/* Overlay "Nivel completat" / "Game over" / "Felicitari" */
void render_overlay(GameState state, int current_level);

#endif /* RENDERER_H */
