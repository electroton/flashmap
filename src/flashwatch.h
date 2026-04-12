/**
 * flashwatch.h - Flash region change watching and monitoring
 *
 * Provides functionality to watch flash regions for changes between
 * two layout states and report which regions were added, removed,
 * or modified (size/address changes).
 */

#ifndef FLASHWATCH_H
#define FLASHWATCH_H

#include "flashlayout.h"
#include "flashregion.h"

#include <stddef.h>

/* Types of changes detected by the watcher */
typedef enum {
    WATCH_ADDED   = 0,  /* Region present in new but not old */
    WATCH_REMOVED = 1,  /* Region present in old but not new */
    WATCH_MOVED   = 2,  /* Region address changed */
    WATCH_RESIZED = 3,  /* Region size changed */
    WATCH_CHANGED = 4   /* Both address and size changed */
} FlashWatchEventType;

/* A single detected change event */
typedef struct {
    FlashWatchEventType type;
    char                name[64];
    uint32_t            old_address;
    uint32_t            new_address;
    uint32_t            old_size;
    uint32_t            new_size;
} FlashWatchEvent;

/* Collection of watch events */
typedef struct {
    FlashWatchEvent *events;
    size_t           count;
    size_t           capacity;
} FlashWatchResult;

/**
 * Compare two layouts and populate result with all detected changes.
 * Returns 0 on success, -1 on allocation failure.
 */
int flashwatch_compare(const FlashLayout *old_layout,
                       const FlashLayout *new_layout,
                       FlashWatchResult  *result);

/**
 * Print a human-readable summary of watch events to stdout.
 */
void flashwatch_print(const FlashWatchResult *result);

/**
 * Free memory allocated within a FlashWatchResult.
 */
void flashwatch_free(FlashWatchResult *result);

#endif /* FLASHWATCH_H */
