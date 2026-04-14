/* ============================================================
 *  world/zombie.c  –  Logica de miscare a zombie-ului
 *
 *  La fiecare ZOMBIE_SPEED secunde, zombie-ul calculeaza
 *  urmatorul pas spre jucator folosind BFS din pathfinder.c
 *  si isi actualizeaza pozitia cu acel pas.
 * ============================================================ */

#include "world/zombie.h"
#include "brain/pathfinder.h"

void zombie_init(Zombie *z, Vec2i pos) {
    z->pos        = pos;
    z->alive      = 1;
    z->move_timer = 0.0f;
}

void zombie_update(Zombie *z, const Maze *m, Vec2i player_pos, float dt) {
    if (!z->alive) return;

    z->move_timer += dt;

    /* Zombie-ul se muta doar cand timer-ul depaseste pragul */
    if (z->move_timer >= ZOMBIE_SPEED) {
        z->move_timer = 0.0f;

        /* Apelam BFS: gaseste urmatorul cel mai bun pas spre jucator */
        z->pos = bfs_next_step(m, z->pos, player_pos);
    }
}
