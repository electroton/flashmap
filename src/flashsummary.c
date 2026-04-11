#include "flashsummary.h"
#include <stdio.h>
#include <string.h>

void flashsummary_init(FlashSummary *summary) {
    if (!summary) return;
    memset(summary, 0, sizeof(FlashSummary));
}

void flashsummary_compute(FlashSummary *summary, const FlashLayout *layout) {
    if (!summary || !layout) return;

    summary->total_regions = layout->region_count;
    summary->total_size = 0;
    summary->used_size = 0;
    summary->free_size = 0;
    summary->largest_free_block = 0;
    summary->smallest_region = UINT32_MAX;
    summary->largest_region = 0;

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *region = &layout->regions[i];
        uint32_t region_size = region->size;
        uint32_t region_used = region->used;
        uint32_t region_free = (region_size >= region_used) ? (region_size - region_used) : 0;

        summary->total_size += region_size;
        summary->used_size += region_used;
        summary->free_size += region_free;

        if (region_free > summary->largest_free_block) {
            summary->largest_free_block = region_free;
        }
        if (region_size < summary->smallest_region) {
            summary->smallest_region = region_size;
        }
        if (region_size > summary->largest_region) {
            summary->largest_region = region_size;
        }
    }

    if (summary->total_size > 0) {
        summary->utilization_percent =
            (float)summary->used_size / (float)summary->total_size * 100.0f;
    } else {
        summary->utilization_percent = 0.0f;
    }

    if (summary->smallest_region == UINT32_MAX) {
        summary->smallest_region = 0;
    }
}

void flashsummary_print(const FlashSummary *summary) {
    if (!summary) return;

    printf("=== Flash Memory Summary ===\n");
    printf("  Total regions     : %u\n", summary->total_regions);
    printf("  Total size        : %u bytes\n", summary->total_size);
    printf("  Used size         : %u bytes\n", summary->used_size);
    printf("  Free size         : %u bytes\n", summary->free_size);
    printf("  Utilization       : %.1f%%\n", summary->utilization_percent);
    printf("  Largest free block: %u bytes\n", summary->largest_free_block);
    printf("  Smallest region   : %u bytes\n", summary->smallest_region);
    printf("  Largest region    : %u bytes\n", summary->largest_region);
    printf("============================\n");
}
