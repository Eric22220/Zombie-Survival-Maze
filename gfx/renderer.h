#ifndef RENDERER_H
#define RENDERER_H

/* ============================================================
 *  gfx/renderer.h  –  Declaratii pentru toate functiile de desen
 *  Foloseste Raylib: DrawRectangle, DrawCircle, DrawText, etc.
 * ============================================================ */

#include "core.h"
#include "maze/maze.h"
#include "world/player.h"
#include "world/zombie.h"

/* Deseneaza grila labirintului (pereti + podea) si cheile inca vii */
void render_maze(const Maze *m, const int keys_alive[NUM_KEYS]);

/* Deseneaza celula de iesire (gri = blocat, verde = deblocat) */
void render_exit(Vec2i exit_pos, int unlocked);

/* Deseneaza jucatorul (cu efect de blink cand e invincibil) */
void render_player(const Player *p);

/* Deseneaza toti zombie-ii activi */
void render_zombies(const Zombie *zombies, int count);

/* Deseneaza bara HUD de jos (HP, chei, hint taste) */
void render_hud(const Player *p, int total_keys);

/* Deseneaza ecranul de overlay (meniu, victorie, game-over) */
void render_overlay(GameState state);

#endif /* RENDERER_H */
