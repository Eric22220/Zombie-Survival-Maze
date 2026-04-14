/* ============================================================
 *  brain/queue.c  –  Implementarea cozii dinamice (linked-list)
 *
 *  Memoria fiecarui nod e alocata cu malloc la enqueue
 *  si eliberata cu free la dequeue sau queue_free.
 *  → zero memory leaks daca apelezi queue_free la final.
 * ============================================================ */

#include <stdlib.h>
#include <assert.h>
#include "brain/queue.h"

Queue *queue_new(void) {
    Queue *q  = (Queue *)malloc(sizeof(Queue));
    q->front  = NULL;
    q->back   = NULL;
    q->size   = 0;
    return q;
}

void queue_push(Queue *q, Vec2i pos) {
    QNode *node = (QNode *)malloc(sizeof(QNode));
    node->pos   = pos;
    node->next  = NULL;

    if (q->back != NULL) {
        q->back->next = node;   /* leaga noul nod la sfarsit    */
    } else {
        q->front = node;        /* coada era goala              */
    }
    q->back = node;
    q->size++;
}

Vec2i queue_pop(Queue *q) {
    assert(!queue_empty(q) && "queue_pop: coada goala!");

    QNode *node = q->front;
    Vec2i  pos  = node->pos;

    q->front = node->next;
    if (q->front == NULL) {
        q->back = NULL;         /* coada a ramas goala          */
    }
    free(node);
    q->size--;
    return pos;
}

int queue_empty(const Queue *q) {
    return (q->front == NULL);
}

void queue_free(Queue *q) {
    /* Eliberam toate nodurile ramase */
    while (!queue_empty(q)) {
        queue_pop(q);
    }
    free(q);
}
