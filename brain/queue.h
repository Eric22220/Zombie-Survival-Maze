#ifndef QUEUE_H
#define QUEUE_H

/* ============================================================
 *  brain/queue.h  –  Coada dinamica (linked-list) pentru BFS
 *
 *  Operatii principale:
 *   enqueue  → adauga la coada  (O(1))
 *   dequeue  → scoate din fata  (O(1))
 *  Fiecare nod tine o pozitie Vec2i in labirint.
 * ============================================================ */

#include "core.h"

/* Nod individual din coada */
typedef struct QNode {
    Vec2i         pos;
    struct QNode *next;
} QNode;

/* Structura principala a cozii */
typedef struct {
    QNode *front;   /* primul element (de unde scoatem)   */
    QNode *back;    /* ultimul element (unde adaugam)      */
    int    size;    /* numarul curent de elemente          */
} Queue;

/* Aloca o coada goala */
Queue *queue_new(void);

/* Adauga o pozitie la sfarsitul cozii */
void   queue_push(Queue *q, Vec2i pos);

/* Scoate si returneaza pozitia din fata cozii */
Vec2i  queue_pop(Queue *q);

/* Returneaza 1 daca coada e goala */
int    queue_empty(const Queue *q);

/* Elibereaza toata memoria cozii (inclusiv nodurile ramase) */
void   queue_free(Queue *q);

#endif /* QUEUE_H */
