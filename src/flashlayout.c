#include "flashlayout.h"
#include <string.h>
#include <stdlib.h>

void flashlayout_init(FlashLayout *layout) {
    if (!layout) return;
    memset(layout, 0, sizeof(FlashLayout));
}

int flashlayout_add_region(FlashLayout *layout, const FlashRegion *region) {
    if (!layout || !region) return -1;
    if (layout->region_count >= FLASHLAYOUT_MAX_REGIONS) return -1;

    /* Check for overlap with existing regions */
    for (size_t i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        uint32_t r_end      = r->start_address + r->size;
        uint32_t new_end    = region->start_address + region->size;
        if (region->start_address < r_end && new_end > r->start_address) {
            return -1; /* overlap detected */
        }
    }

    layout->regions[layout->region_count++] = *region;
    return 0;
}

uint32_t flashlayout_used_bytes(const FlashLayout *layout) {
    if (!layout) return 0;
    uint32_t used = 0;
    for (size_t i = 0; i < layout->region_count; i++) {
        used += layout->regions[i].size;
    }
    return used;
}

uint32_t flashlayout_free_bytes(const FlashLayout *layout) {
    if (!layout) return 0;
    uint32_t used = flashlayout_used_bytes(layout);
    if (used > layout->total_flash_size) return 0;
    return layout->total_flash_size - used;
}

static int region_cmp(const void *a, const void *b) {
    const FlashRegion *ra = (const FlashRegion *)a;
    const FlashRegion *rb = (const FlashRegion *)b;
    if (ra->start_address < rb->start_address) return -1;
    if (ra->start_address > rb->start_address) return  1;
    return 0;
}

void flashlayout_sort(FlashLayout *layout) {
    if (!layout || layout->region_count == 0) return;
    qsort(layout->regions, layout->region_count, sizeof(FlashRegion), region_cmp);
}

int flashlayout_has_overlaps(const FlashLayout *layout) {
    if (!layout || layout->region_count < 2) return 0;
    for (size_t i = 0; i < layout->region_count - 1; i++) {
        for (size_t j = i + 1; j < layout->region_count; j++) {
            const FlashRegion *a = &layout->regions[i];
            const FlashRegion *b = &layout->regions[j];
            uint32_t a_end = a->start_address + a->size;
            uint32_t b_end = b->start_address + b->size;
            if (a->start_address < b_end && a_end > b->start_address) {
                return 1;
            }
        }
    }
    return 0;
}
