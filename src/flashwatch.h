/*
 * flashwatch.h - Watch regions for changes across snapshots
 */

#ifndef FLASHWATCH_H
#define FLASHWATCH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "flashlayout.h"
#include "flashregion.h"

#define FLASH_WATCH_NAME_MAX 64

typedef struct {
    char     region_name[FLASH_WATCH_NAME_MAX];
    uint32_t base;
    uint32_t size;
    uint32_t prev_size;
    bool     triggered;
} FlashWatchEntry;

typedef struct {
    FlashWatchEntry *entries;
    size_t           count;
    size_t           capacity;
} FlashWatch;

/* Lifecycle */
FlashWatch *flashwatch_create(void);
void        flashwatch_destroy(FlashWatch *watch);

/* Manage watched regions */
int flashwatch_add(FlashWatch *watch, const char *region_name,
                   uint32_t base, uint32_t size);
int flashwatch_remove(FlashWatch *watch, const char *region_name);
void flashwatch_reset(FlashWatch *watch);

/* Check layout against watched regions; returns number of triggered watches */
int flashwatch_check(FlashWatch *watch, const FlashLayout *layout);

/* Retrieve a specific watch entry by name */
FlashWatchEntry *flashwatch_get(FlashWatch *watch, const char *region_name);

#endif /* FLASHWATCH_H */
