#include "flashwatermark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WATERMARK_INITIAL_CAP 8

void flash_watermark_set_init(FlashWatermarkSet *set) {
    if (!set) return;
    set->marks    = NULL;
    set->count    = 0;
    set->capacity = 0;
}

void flash_watermark_set_free(FlashWatermarkSet *set) {
    if (!set) return;
    free(set->marks);
    set->marks    = NULL;
    set->count    = 0;
    set->capacity = 0;
}

int flash_watermark_record(FlashWatermarkSet *set,
                           const char *name,
                           uint32_t base,
                           uint32_t capacity,
                           uint32_t used) {
    if (!set || !name) return -1;

    /* Search for existing entry */
    for (size_t i = 0; i < set->count; i++) {
        if (strcmp(set->marks[i].name, name) == 0) {
            set->marks[i].current_used = used;
            if (used > set->marks[i].peak_used) {
                set->marks[i].peak_used = used;
                set->marks[i].peak_pct  = (capacity > 0)
                    ? (float)used / (float)capacity * 100.0f
                    : 0.0f;
            }
            return 0;
        }
    }

    /* Grow if needed */
    if (set->count >= set->capacity) {
        size_t new_cap = (set->capacity == 0)
            ? WATERMARK_INITIAL_CAP
            : set->capacity * 2;
        FlashWatermark *tmp = realloc(set->marks,
                                      new_cap * sizeof(FlashWatermark));
        if (!tmp) return -1;
        set->marks    = tmp;
        set->capacity = new_cap;
    }

    FlashWatermark *m = &set->marks[set->count++];
    memset(m, 0, sizeof(*m));
    strncpy(m->name, name, sizeof(m->name) - 1);
    m->base         = base;
    m->capacity     = capacity;
    m->current_used = used;
    m->peak_used    = used;
    m->peak_pct     = (capacity > 0)
        ? (float)used / (float)capacity * 100.0f
        : 0.0f;
    return 0;
}

const FlashWatermark *flash_watermark_find(const FlashWatermarkSet *set,
                                           const char *name) {
    if (!set || !name) return NULL;
    for (size_t i = 0; i < set->count; i++) {
        if (strcmp(set->marks[i].name, name) == 0)
            return &set->marks[i];
    }
    return NULL;
}

void flash_watermark_reset_peaks(FlashWatermarkSet *set) {
    if (!set) return;
    for (size_t i = 0; i < set->count; i++) {
        set->marks[i].peak_used = set->marks[i].current_used;
        uint32_t cap = set->marks[i].capacity;
        set->marks[i].peak_pct = (cap > 0)
            ? (float)set->marks[i].peak_used / (float)cap * 100.0f
            : 0.0f;
    }
}

void flash_watermark_print(const FlashWatermarkSet *set) {
    if (!set || set->count == 0) {
        printf("No watermark data available.\n");
        return;
    }
    printf("%-20s %10s %10s %10s %8s\n",
           "Region", "Base", "Capacity", "Peak", "Peak%");
    printf("%-20s %10s %10s %10s %8s\n",
           "------", "----", "--------", "----", "-----");
    for (size_t i = 0; i < set->count; i++) {
        const FlashWatermark *m = &set->marks[i];
        printf("%-20s 0x%08X %10u %10u %7.1f%%\n",
               m->name, m->base, m->capacity, m->peak_used, m->peak_pct);
    }
}
