#include "flashlabel.h"
#include <string.h>
#include <stdio.h>

void flashlabel_set_init(FlashLabelSet *set) {
    if (!set) return;
    memset(set, 0, sizeof(*set));
}

int flashlabel_add(FlashLabelSet *set, const char *name, uint32_t address,
                   uint32_t size, FlashLabelKind kind, const char *note) {
    if (!set || !name || set->count >= FLASHLABEL_MAX_LABELS) return -1;
    /* Reject duplicates */
    if (flashlabel_find(set, name)) return -1;

    FlashLabel *lbl = &set->entries[set->count];
    strncpy(lbl->name, name, FLASHLABEL_MAX_NAME - 1);
    lbl->name[FLASHLABEL_MAX_NAME - 1] = '\0';
    if (note) {
        strncpy(lbl->note, note, FLASHLABEL_MAX_NOTE - 1);
        lbl->note[FLASHLABEL_MAX_NOTE - 1] = '\0';
    }
    lbl->address = address;
    lbl->size    = size;
    lbl->kind    = kind;
    lbl->pinned  = 0;
    set->count++;
    return 0;
}

int flashlabel_remove(FlashLabelSet *set, const char *name) {
    if (!set || !name) return -1;
    for (size_t i = 0; i < set->count; i++) {
        if (strcmp(set->entries[i].name, name) == 0) {
            if (set->entries[i].pinned) return -1; /* cannot remove pinned */
            set->entries[i] = set->entries[set->count - 1];
            set->count--;
            return 0;
        }
    }
    return -1;
}

const FlashLabel *flashlabel_find(const FlashLabelSet *set, const char *name) {
    if (!set || !name) return NULL;
    for (size_t i = 0; i < set->count; i++) {
        if (strcmp(set->entries[i].name, name) == 0)
            return &set->entries[i];
    }
    return NULL;
}

const FlashLabel *flashlabel_at(const FlashLabelSet *set, uint32_t address) {
    if (!set) return NULL;
    for (size_t i = 0; i < set->count; i++) {
        const FlashLabel *l = &set->entries[i];
        if (address >= l->address && address < l->address + l->size)
            return l;
    }
    return NULL;
}

int flashlabel_from_region(FlashLabelSet *set, const FlashRegion *region,
                            FlashLabelKind kind) {
    if (!set || !region) return -1;
    return flashlabel_add(set, region->name, region->start,
                          region->size, kind, NULL);
}

size_t flashlabel_overlapping(const FlashLabelSet *set, uint32_t addr,
                               uint32_t size, FlashLabel *out, size_t out_max) {
    if (!set || !out || out_max == 0) return 0;
    size_t found = 0;
    uint32_t end = addr + size;
    for (size_t i = 0; i < set->count && found < out_max; i++) {
        const FlashLabel *l = &set->entries[i];
        uint32_t lend = l->address + l->size;
        if (l->address < end && lend > addr)
            out[found++] = *l;
    }
    return found;
}

void flashlabel_pin(FlashLabelSet *set, const char *name) {
    if (!set || !name) return;
    for (size_t i = 0; i < set->count; i++) {
        if (strcmp(set->entries[i].name, name) == 0) {
            set->entries[i].pinned = 1;
            return;
        }
    }
}

void flashlabel_unpin(FlashLabelSet *set, const char *name) {
    if (!set || !size_t i = 0; i < set->count; i++) {
        if (strcmp(set->entries[i].name, name) == 0) {
            set->entries[i].pinned = 0;
            return;
        }
    }
}
