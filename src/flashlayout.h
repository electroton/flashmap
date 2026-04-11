#ifndef FLASHLAYOUT_H
#define FLASHLAYOUT_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#define FLASHLAYOUT_MAX_REGIONS 64

typedef struct {
    FlashRegion regions[FLASHLAYOUT_MAX_REGIONS];
    size_t      region_count;
    uint32_t    total_flash_size;
    uint32_t    base_address;
} FlashLayout;

/**
 * Initialize a FlashLayout to an empty state.
 */
void flashlayout_init(FlashLayout *layout);

/**
 * Add a region to the layout. Returns 0 on success, -1 if the layout is full
 * or the region overlaps an existing one.
 */
int flashlayout_add_region(FlashLayout *layout, const FlashRegion *region);

/**
 * Compute total used bytes across all regions.
 */
uint32_t flashlayout_used_bytes(const FlashLayout *layout);

/**
 * Compute total free bytes given the total flash size.
 */
uint32_t flashlayout_free_bytes(const FlashLayout *layout);

/**
 * Sort regions in the layout by start address (ascending).
 */
void flashlayout_sort(FlashLayout *layout);

/**
 * Return 1 if any regions overlap, 0 otherwise.
 */
int flashlayout_has_overlaps(const FlashLayout *layout);

#endif /* FLASHLAYOUT_H */
