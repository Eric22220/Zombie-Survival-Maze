/* ============================================================
 *  world/player.c  –  Input de tastatura si miscare jucator
 * ============================================================ */

#include <raylib.h>
#include "world/player.h"

Player player_new(Vec2i start) {
    Player p;
    p.pos              = start;
    p.health           = PLAYER_HEALTH;
    p.keys_collected   = 0;
    p.invincible_timer = 0;
    return p;
}

void player_update(Player *p, const Maze *m) {
    /* Decrementam timer-ul de invincibilitate */
    if (p->invincible_timer > 0) {
        p->invincible_timer--;
    }

    /* Calculam noua pozitie dorita pe baza tastelor apasate */
    Vec2i next = p->pos;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    next.y--;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  next.y++;
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  next.x--;
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) next.x++;

    /* Mutam jucatorul DOAR daca destinatia e celula libera (FLOOR) */
    if (maze_is_floor(m, next.x, next.y)) {
        p->pos = next;
    }
}

void player_collect_key(Player *p) {
    p->keys_collected++;
}

int player_is_alive(const Player *p) {
    return (p->health > 0);
}
