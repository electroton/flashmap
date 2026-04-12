/*
 * flashwatch.c - Watch regions for changes across snapshots
 */

#include "flashwatch.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashWatch *flashwatch_create(void) {
    FlashWatch *watch = calloc(1, sizeof(FlashWatch));
    if (!watch) return NULL;
    watch->count = 0;
    watch->capacity = 8;
    watch->entries = calloc(watch->capacity, sizeof(FlashWatchEntry));
    if (!watch->entries) {
        free(watch);
        return NULL;
    }
    return watch;
}

void flashwatch_destroy(FlashWatch *watch) {
    if (!watch) return;
    free(watch->entries);
    free(watch);
}

int flashwatch_add(FlashWatch *watch, const char *region_name, uint32_t base, uint32_t size) {
    if (!watch || !region_name) return -1;
    if (watch->count >= watch->capacity) {
        size_t new_cap = watch->capacity * 2;
        FlashWatchEntry *tmp = realloc(watch->entries, new_cap * sizeof(FlashWatchEntry));
        if (!tmp) return -1;
        watch->entries = tmp;
        watch->capacity = new_cap;
    }
    FlashWatchEntry *e = &watch->entries[watch->count++];
    strncpy(e->region_name, region_name, FLASH_WATCH_NAME_MAX - 1);
    e->region_name[FLASH_WATCH_NAME_MAX - 1] = '\0';
    e->base = base;
    e->size = size;
    e->triggered = false;
    e->prev_size = size;
    return 0;
}

int flashwatch_remove(FlashWatch *watch, const char *region_name) {
    if (!watch || !region_name) return -1;
    for (size_t i = 0; i < watch->count; i++) {
        if (strcmp(watch->entries[i].region_name, region_name) == 0) {
            memmove(&watch->entries[i], &watch->entries[i + 1],
                    (watch->count - i - 1) * sizeof(FlashWatchEntry));
            watch->count--;
            return 0;
        }
    }
    return -1;
}

int flashwatch_check(FlashWatch *watch, const FlashLayout *layout) {
    if (!watch || !layout) return -1;
    int triggered = 0;
    for (size_t i = 0; i < watch->count; i++) {
        FlashWatchEntry *e = &watch->entries[i];
        e->triggered = false;
        for (size_t j = 0; j < layout->count; j++) {
            const FlashRegion *r = &layout->regions[j];
            if (strcmp(r->name, e->region_name) == 0) {
                if (r->size != e->prev_size || r->base != e->base) {
                    e->triggered = true;
                    e->prev_size = r->size;
                    e->base = r->base;
                    triggered++;
                }
                break;
            }
        }
    }
    return triggered;
}

FlashWatchEntry *flashwatch_get(FlashWatch *watch, const char *region_name) {
    if (!watch || !region_name) return NULL;
    for (size_t i = 0; i < watch->count; i++) {
        if (strcmp(watch->entries[i].region_name, region_name) == 0)
            return &watch->entries[i];
    }
    return NULL;
}

void flashwatch_reset(FlashWatch *watch) {
    if (!watch) return;
    for (size_t i = 0; i < watch->count; i++)
        watch->entries[i].triggered = false;
}
