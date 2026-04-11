#include "flashgap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Comparator for sorting regions by start address */
static int region_cmp(const void *a, const void *b) {
    const FlashRegion *ra = (const FlashRegion *)a;
    const FlashRegion *rb = (const FlashRegion *)b;
    if (ra->start < rb->start) return -1;
    if (ra->start > rb->start) return  1;
    return 0;
}

FlashGapResult flash_gap_find(const FlashLayout *layout,
                              uint32_t base_addr,
                              uint32_t total_size,
                              FlashGap *gaps,
                              size_t max_gaps) {
    if (!layout || !gaps || total_size == 0) {
        return FLASH_GAP_ERR_NULL;
    }

    /* Copy and sort regions by start address */
    FlashRegion sorted[64];
    size_t n = layout->region_count < 64 ? layout->region_count : 64;
    memcpy(sorted, layout->regions, n * sizeof(FlashRegion));
    qsort(sorted, n, sizeof(FlashRegion), region_cmp);

    size_t gap_count = 0;
    uint32_t cursor = base_addr;
    uint32_t end_addr = base_addr + total_size;

    for (size_t i = 0; i < n; i++) {
        uint32_t r_start = sorted[i].start;
        uint32_t r_end   = sorted[i].start + sorted[i].size;

        if (r_start < base_addr || r_start >= end_addr) continue;

        if (r_start > cursor) {
            if (gap_count >= max_gaps) return FLASH_GAP_ERR_OVERFLOW;
            gaps[gap_count].start  = cursor;
            gaps[gap_count].size   = r_start - cursor;
            gaps[gap_count].before = (i > 0) ? &layout->regions[i - 1] : NULL;
            gaps[gap_count].after  = &layout->regions[i];
            gap_count++;
        }
        if (r_end > cursor) cursor = r_end;
    }

    /* Gap after last region */
    if (cursor < end_addr) {
        if (gap_count >= max_gaps) return FLASH_GAP_ERR_OVERFLOW;
        gaps[gap_count].start  = cursor;
        gaps[gap_count].size   = end_addr - cursor;
        gaps[gap_count].before = n > 0 ? &layout->regions[n - 1] : NULL;
        gaps[gap_count].after  = NULL;
        gap_count++;
    }

    return (FlashGapResult)gap_count;
}

uint32_t flash_gap_total(const FlashGap *gaps, size_t count) {
    uint32_t total = 0;
    for (size_t i = 0; i < count; i++) total += gaps[i].size;
    return total;
}

bool flash_gap_has_large(const FlashGap *gaps, size_t count, uint32_t min_size) {
    for (size_t i = 0; i < count; i++) {
        if (gaps[i].size >= min_size) return true;
    }
    return false;
}

void flash_gap_print(const FlashGap *gaps, size_t count, FILE *out) {
    if (!gaps || !out) return;
    fprintf(out, "Flash Gaps (%zu found):\n", count);
    for (size_t i = 0; i < count; i++) {
        fprintf(out, "  GAP [0x%08X - 0x%08X] size=0x%X (%u bytes)\n",
                gaps[i].start,
                gaps[i].start + gaps[i].size,
                gaps[i].size, gaps[i].size);
    }
}
