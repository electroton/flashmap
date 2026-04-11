#ifndef FLASHSTATS_H
#define FLASHSTATS_H

#include "flashlayout.h"
#include "flashregion.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Aggregated statistics for a flash memory layout.
 */
typedef struct {
    size_t   region_count;       /* Total number of regions */
    uint32_t total_size;         /* Sum of all region sizes (bytes) */
    uint32_t used_size;          /* Bytes occupied by non-gap regions */
    uint32_t free_size;          /* Bytes in gap regions */
    uint32_t largest_region;     /* Size of the largest region */
    uint32_t smallest_region;    /* Size of the smallest region */
    double   avg_region_size;    /* Average region size */
    double   utilization_pct;    /* used_size / total_size * 100 */
    uint32_t address_span;       /* max_end - min_start across all regions */
    uint32_t min_address;        /* Lowest start address found */
    uint32_t max_address;        /* Highest end address found */
} FlashStats;

/**
 * Compute statistics for the given layout.
 * Returns 0 on success, -1 if layout is NULL or has no regions.
 */
int flashstats_compute(const FlashLayout *layout, FlashStats *out);

/**
 * Print a human-readable statistics report to stdout.
 */
void flashstats_print(const FlashStats *stats);

/**
 * Return the region with the highest utilization relative to its capacity.
 * Returns NULL if layout is empty.
 */
const FlashRegion *flashstats_busiest_region(const FlashLayout *layout);

/**
 * Fill buf with a one-line ASCII utilization bar of width `bar_width`.
 * buf must be at least bar_width + 3 bytes.
 */
void flashstats_utilization_bar(const FlashStats *stats, char *buf, int bar_width);

#endif /* FLASHSTATS_H */
