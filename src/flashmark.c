#include "flashmark.h"
#include <string.h>
#include <stdlib.h>

void flashmark_init(FlashMarkSet *set)
{
    if (!set) return;
    memset(set, 0, sizeof(*set));
    set->count = 0;
}

int flashmark_add(FlashMarkSet *set, const char *name, uint32_t address,
                  FlashMarkType type, int region_index)
{
    if (!set || !name || set->count >= FLASHMARK_MAX) return -1;

    /* Reject duplicate names */
    if (flashmark_find(set, name)) return -1;

    FlashMark *m = &set->marks[set->count];
    strncpy(m->name, name, FLASHMARK_NAME_LEN - 1);
    m->name[FLASHMARK_NAME_LEN - 1] = '\0';
    m->address      = address;
    m->type         = type;
    m->region_index = region_index;
    set->count++;
    return 0;
}

const FlashMark *flashmark_find(const FlashMarkSet *set, const char *name)
{
    if (!set || !name) return NULL;
    for (size_t i = 0; i < set->count; i++) {
        if (strncmp(set->marks[i].name, name, FLASHMARK_NAME_LEN) == 0)
            return &set->marks[i];
    }
    return NULL;
}

int flashmark_remove(FlashMarkSet *set, const char *name)
{
    if (!set || !name) return -1;
    for (size_t i = 0; i < set->count; i++) {
        if (strncmp(set->marks[i].name, name, FLASHMARK_NAME_LEN) == 0) {
            /* Shift remaining entries down */
            memmove(&set->marks[i], &set->marks[i + 1],
                    (set->count - i - 1) * sizeof(FlashMark));
            set->count--;
            return 0;
        }
    }
    return -1;
}

size_t flashmark_in_range(const FlashMarkSet *set, uint32_t start,
                          uint32_t size, const FlashMark **out, size_t out_max)
{
    if (!set || !out || out_max == 0) return 0;
    size_t found = 0;
    uint32_t end = start + size;
    for (size_t i = 0; i < set->count && found < out_max; i++) {
        uint32_t addr = set->marks[i].address;
        if (addr >= start && addr < end)
            out[found++] = &set->marks[i];
    }
    return found;
}

static int mark_cmp(const void *a, const void *b)
{
    const FlashMark *ma = (const FlashMark *)a;
    const FlashMark *mb = (const FlashMark *)b;
    if (ma->address < mb->address) return -1;
    if (ma->address > mb->address) return  1;
    return 0;
}

void flashmark_sort(FlashMarkSet *set)
{
    if (!set || set->count == 0) return;
    qsort(set->marks, set->count, sizeof(FlashMark), mark_cmp);
}
