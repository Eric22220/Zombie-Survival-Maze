#ifndef SAVE_H
#define SAVE_H

/* ============================================================
 *  save.h  –  Persistenta progresului in fisier (save.dat)
 *
 *  Format binar simplu:
 *    [magic: uint32]                ZMZ1 = 0x5A4D5A31
 *    [max_unlocked: int]            1..MAX_LEVELS
 *    [levels_completed: int x MAX_LEVELS]
 *
 *  Daca fisierul lipseste sau e corupt, se incarca default:
 *  nivelul 1 deblocat, nimic completat.
 * ============================================================ */

#include "core.h"

typedef struct {
    int max_unlocked;                       /* cel mai inalt nivel jucabil */
    int levels_completed[MAX_LEVELS];       /* 1 = completat               */
} SaveData;

/* Incarca progresul din save.dat (sau default daca lipseste/corupt) */
SaveData save_load(void);

/* Scrie progresul in save.dat */
void     save_write(const SaveData *s);

/* Marcheaza nivelul ca terminat si deblocheaza urmatorul.
   Salveaza automat in fisier. */
void     save_mark_complete(SaveData *s, int level);

#endif /* SAVE_H */
