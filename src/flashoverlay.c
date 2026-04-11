#include "flashoverlay.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashOverlayResult flash_overlay_detect(const FlashLayout *a, const FlashLayout *b,
                                         FlashOverlap *overlaps, size_t max_overlaps) {
    if (!a || !b || !overlaps || max_overlaps == 0) {
        return FLASH_OVERLAY_ERR_INVALID;
    }

    size_t count = 0;

    for (size_t i = 0; i < a->region_count; i++) {
        for (size_t j = 0; j < b->region_count; j++) {
            const FlashRegion *ra = &a->regions[i];
            const FlashRegion *rb = &b->regions[j];

            uint32_t a_end = ra->start + ra->size;
            uint32_t b_end = rb->start + rb->size;

            if (ra->start < b_end && a_end > rb->start) {
                if (count >= max_overlaps) {
                    return FLASH_OVERLAY_ERR_OVERFLOW;
                }
                overlaps[count].region_a = ra;
                overlaps[count].region_b = rb;
                overlaps[count].overlap_start = ra->start > rb->start ? ra->start : rb->start;
                uint32_t min_end = a_end < b_end ? a_end : b_end;
                overlaps[count].overlap_size = min_end - overlaps[count].overlap_start;
                count++;
            }
        }
    }

    return (FlashOverlayResult)count;
}

bool flash_overlay_has_conflict(const FlashLayout *layout) {
    if (!layout) return false;

    for (size_t i = 0; i < layout->region_count; i++) {
        for (size_t j = i + 1; j < layout->region_count; j++) {
            const FlashRegion *ra = &layout->regions[i];
            const FlashRegion *rb = &layout->regions[j];
            uint32_t a_end = ra->start + ra->size;
            uint32_t b_end = rb->start + rb->size;
            if (ra->start < b_end && a_end > rb->start) {
                return true;
            }
        }
    }
    return false;
}

void flash_overlay_print(const FlashOverlap *overlaps, size_t count, FILE *out) {
    if (!overlaps || !out) return;
    for (size_t i = 0; i < count; i++) {
        fprintf(out, "OVERLAP: '%s' [0x%08X] vs '%s' [0x%08X] -> 0x%08X..0x%08X (%u bytes)\n",
                overlaps[i].region_a->name,
                overlaps[i].region_a->start,
                overlaps[i].region_b->name,
                overlaps[i].region_b->start,
                overlaps[i].overlap_start,
                overlaps[i].overlap_start + overlaps[i].overlap_size,
                overlaps[i].overlap_size);
    }
}
