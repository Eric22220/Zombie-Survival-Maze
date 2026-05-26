/* ============================================================
 *  save.c  –  Citire/scriere save.dat (format binar mic)
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "save.h"

#define SAVE_FILE   "save.dat"
#define SAVE_MAGIC  0x5A4D5A31u   /* "ZMZ1" – format v1 */

static SaveData save_default(void) {
    SaveData s;
    memset(&s, 0, sizeof(s));
    s.max_unlocked = 1;     /* doar nivelul 1 e deblocat initial */
    return s;
}

SaveData save_load(void) {
    SaveData s = save_default();

    FILE *f = fopen(SAVE_FILE, "rb");
    if (!f) return s;       /* fisier inexistent → default */

    unsigned int magic = 0;
    if (fread(&magic, sizeof(magic), 1, f) != 1 || magic != SAVE_MAGIC) {
        fclose(f);
        return save_default();
    }

    int max_unlocked = 0;
    int completed[MAX_LEVELS] = {0};

    if (fread(&max_unlocked, sizeof(int), 1, f) != 1 ||
        fread(completed, sizeof(int), MAX_LEVELS, f) != (size_t)MAX_LEVELS)
    {
        fclose(f);
        return save_default();
    }
    fclose(f);

    /* Sanity clamping */
    if (max_unlocked < 1)          max_unlocked = 1;
    if (max_unlocked > MAX_LEVELS) max_unlocked = MAX_LEVELS;

    s.max_unlocked = max_unlocked;
    for (int i = 0; i < MAX_LEVELS; i++) {
        s.levels_completed[i] = (completed[i] != 0) ? 1 : 0;
    }
    return s;
}

void save_write(const SaveData *s) {
    FILE *f = fopen(SAVE_FILE, "wb");
    if (!f) return;     /* esec silentios: jocul merge si fara save */

    unsigned int magic = SAVE_MAGIC;
    fwrite(&magic,            sizeof(unsigned int), 1,          f);
    fwrite(&s->max_unlocked,  sizeof(int),          1,          f);
    fwrite(s->levels_completed, sizeof(int),        MAX_LEVELS, f);

    fclose(f);
}

void save_mark_complete(SaveData *s, int level) {
    if (level < 1 || level > MAX_LEVELS) return;

    s->levels_completed[level - 1] = 1;

    /* Deblocam urmatorul nivel (daca exista si daca nu e deja deblocat) */
    int next = level + 1;
    if (next <= MAX_LEVELS && s->max_unlocked < next) {
        s->max_unlocked = next;
    }

    save_write(s);
}
