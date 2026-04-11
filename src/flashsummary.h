/**
 * flashsummary.h - Flash memory usage summary and statistics
 */
#ifndef FLASHSUMMARY_H
#define FLASHSUMMARY_H

#include "flashlayout.h"

/**
 * Aggregated statistics for a flash layout.
 */
typedef struct {
    size_t total_capacity;     /* Total flash capacity in bytes */
    size_t total_used;         /* Total bytes used across all regions */
    size_t total_free;         /* Total bytes free across all regions */
    double usage_percent;      /* Overall usage as a percentage */
    int    region_count;       /* Number of regions in the layout */
    int    full_regions;       /* Regions at >= 100% capacity */
    int    critical_regions;   /* Regions at >= 90% capacity */
    size_t largest_free_block; /* Largest contiguous free space */
    const FlashRegion *most_used_region;  /* Pointer to most utilized region */
    const FlashRegion *least_used_region; /* Pointer to least utilized region */
} FlashSummary;

/**
 * Compute a summary of the given flash layout.
 *
 * @param layout  Pointer to a populated FlashLayout.
 * @param summary Output parameter filled with computed statistics.
 * @return        0 on success, -1 if layout or summary is NULL.
 */
int flash_summary_compute(const FlashLayout *layout, FlashSummary *summary);

/**
 * Print a human-readable summary to stdout.
 *
 * @param summary Pointer to a computed FlashSummary.
 */
void flash_summary_print(const FlashSummary *summary);

#endif /* FLASHSUMMARY_H */
