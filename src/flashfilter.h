#ifndef FLASHFILTER_H
#define FLASHFILTER_H

#include "flashlayout.h"
#include "flashregion.h"
#include <stdint.h>

/* Filter criteria flags */
#define FILTER_BY_NAME     (1 << 0)
#define FILTER_BY_MIN_SIZE (1 << 1)
#define FILTER_BY_MAX_SIZE (1 << 2)
#define FILTER_BY_USAGE    (1 << 3)

typedef struct {
    uint32_t flags;
    const char *name_pattern;   /* substring match, NULL = any */
    uint32_t min_size;          /* inclusive lower bound in bytes */
    uint32_t max_size;          /* inclusive upper bound in bytes */
    float    min_usage_pct;     /* 0.0 – 100.0 */
    float    max_usage_pct;
} FlashFilter;

/**
 * Apply filter criteria to a layout and return a new layout containing
 * only the regions that match.  The caller owns the returned layout and
 * must call flashlayout_free() on it.
 *
 * Returns NULL on allocation failure or if layout is NULL.
 */
FlashLayout *flashfilter_apply(const FlashLayout *layout,
                               const FlashFilter *filter);

/**
 * Initialise a FlashFilter with permissive defaults (matches everything).
 * After this call all flags are cleared, name_pattern is NULL, size bounds
 * are 0 and UINT32_MAX respectively, and usage bounds span 0.0–100.0.
 */
void flashfilter_init(FlashFilter *filter);

/**
 * Return non-zero if the given region matches all active criteria in filter.
 * This is the per-region predicate used internally by flashfilter_apply() but
 * is exposed here to allow callers to test individual regions without
 * constructing a full layout.
 *
 * Both region and filter must be non-NULL.
 */
int flashfilter_matches(const FlashRegion *region, const FlashFilter *filter);

#endif /* FLASHFILTER_H */
