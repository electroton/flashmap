/**
 * flashhistory.h - Flash region history tracking module
 *
 * Tracks a time-ordered sequence of flash layout snapshots and provides
 * utilities for querying and iterating over historical states.
 */

#ifndef FLASHHISTORY_H
#define FLASHHISTORY_H

#include "flashregion.h"
#include "flashlayout.h"

#include <stddef.h>
#include <time.h>

#define FLASHHISTORY_MAX_ENTRIES 64
#define FLASHHISTORY_LABEL_LEN   64

typedef struct {
    FlashLayout  layout;
    time_t       timestamp;
    char         label[FLASHHISTORY_LABEL_LEN];
} FlashHistoryEntry;

typedef struct {
    FlashHistoryEntry entries[FLASHHISTORY_MAX_ENTRIES];
    size_t            count;
} FlashHistory;

/**
 * Initialize an empty history object.
 */
void flashhistory_init(FlashHistory *history);

/**
 * Record a layout snapshot into the history with an optional label.
 * Returns 0 on success, -1 if history is full.
 */
int flashhistory_record(FlashHistory *history,
                        const FlashLayout *layout,
                        const char *label);

/**
 * Retrieve an entry by index (0 = oldest).
 * Returns NULL if index is out of range.
 */
const FlashHistoryEntry *flashhistory_get(const FlashHistory *history,
                                          size_t index);

/**
 * Find the most recent entry whose label matches (exact string match).
 * Returns NULL if not found.
 */
const FlashHistoryEntry *flashhistory_find_by_label(
        const FlashHistory *history, const char *label);

/**
 * Return the number of recorded entries.
 */
size_t flashhistory_count(const FlashHistory *history);

/**
 * Clear all history entries.
 */
void flashhistory_clear(FlashHistory *history);

#endif /* FLASHHISTORY_H */
