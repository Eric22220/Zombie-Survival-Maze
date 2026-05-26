/* ============================================================
 *  world/zombie.c  –  Logica de miscare a zombie-ului
 *
 *  La fiecare zombie_speed secunde, zombie-ul calculeaza
 *  urmatorul pas spre jucator folosind BFS din pathfinder.c.
 *  Daca e frozen (power-up FREEZE activ), nu se misca deloc.
 * ============================================================ */

#include "world/zombie.h"
#include "brain/pathfinder.h"

void zombie_init(Zombie *z, Vec2i pos) {
    z->pos        = pos;
    z->spawn      = pos;
    z->alive      = 1;
    z->move_timer = 0.0f;
}

void zombie_update(Zombie *z, const Maze *m, Vec2i player_pos,
                   float dt, float zombie_speed, int frozen)
{
    if (!z->alive)  return;
    if (frozen) {
        /* Cand e inghet, oprim si timer-ul, ca sa nu se miste "pe loc"
           imediat ce expira power-up-ul. */
        return;
    }

    z->move_timer += dt;
    if (z->move_timer >= zombie_speed) {
        z->move_timer = 0.0f;
        z->pos = bfs_next_step(m, z->pos, player_pos);
    }
}
