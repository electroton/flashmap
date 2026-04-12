/**
 * flashnotify.h - Flash memory threshold notification system
 *
 * Provides functionality to register usage thresholds on flash regions
 * and emit notifications when those thresholds are crossed.
 */

#ifndef FLASHNOTIFY_H
#define FLASHNOTIFY_H

#include "flashregion.h"
#include <stdint.h>
#include <stdbool.h>

#define FLASHNOTIFY_MAX_RULES 32
#define FLASHNOTIFY_NAME_LEN  64

typedef enum {
    NOTIFY_LEVEL_INFO    = 0,
    NOTIFY_LEVEL_WARNING = 1,
    NOTIFY_LEVEL_CRITICAL = 2
} FlashNotifyLevel;

typedef struct {
    char            region_name[FLASHNOTIFY_NAME_LEN];
    uint8_t         threshold_pct;   /* 0-100 percent used */
    FlashNotifyLevel level;
    bool            triggered;
} FlashNotifyRule;

typedef struct {
    FlashNotifyRule rules[FLASHNOTIFY_MAX_RULES];
    int             rule_count;
} FlashNotifySet;

/**
 * Initialize an empty notification rule set.
 */
void flashnotify_init(FlashNotifySet *set);

/**
 * Add a threshold rule for a named region.
 * Returns 0 on success, -1 if the set is full.
 */
int flashnotify_add_rule(FlashNotifySet *set,
                         const char *region_name,
                         uint8_t threshold_pct,
                         FlashNotifyLevel level);

/**
 * Evaluate all rules against the given regions array.
 * Marks each rule as triggered if the region usage meets or exceeds threshold.
 */
void flashnotify_evaluate(FlashNotifySet *set,
                          const FlashRegion *regions,
                          int region_count);

/**
 * Print triggered notifications to stdout.
 */
void flashnotify_report(const FlashNotifySet *set);

/**
 * Return the highest severity level among triggered rules,
 * or NOTIFY_LEVEL_INFO if none triggered.
 */
FlashNotifyLevel flashnotify_max_level(const FlashNotifySet *set);

#endif /* FLASHNOTIFY_H */
