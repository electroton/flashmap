/**
 * flashsplit.h - Flash region splitting utilities
 *
 * Provides functionality to split flash regions into smaller
 * sub-regions based on size or address boundaries.
 */

#ifndef FLASHSPLIT_H
#define FLASHSPLIT_H

#include "flashregion.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Result of a split operation containing an array of sub-regions.
 */
typedef struct {
    FlashRegion *regions;  /* Dynamically allocated array of regions */
    size_t       count;    /* Number of regions produced */
} FlashSplitResult;

/**
 * Split a region into equal-sized chunks.
 *
 * @param region     Source region to split
 * @param chunk_size Size of each chunk in bytes (must be > 0)
 * @return           FlashSplitResult with allocated regions, or {NULL, 0} on error
 *                   Caller must free result.regions
 */
FlashSplitResult flashsplit_by_size(const FlashRegion *region, uint32_t chunk_size);

/**
 * Split a region at a specific address boundary.
 *
 * @param region   Source region to split
 * @param address  Address at which to split (must be within region)
 * @param lower    Output: lower sub-region [region->start, address)
 * @param upper    Output: upper sub-region [address, region->end]
 * @return         0 on success, -1 if address is out of range
 */
int flashsplit_at_address(const FlashRegion *region, uint32_t address,
                          FlashRegion *lower, FlashRegion *upper);

/**
 * Free resources held by a FlashSplitResult.
 *
 * @param result  Pointer to result to free
 */
void flashsplit_result_free(FlashSplitResult *result);

#endif /* FLASHSPLIT_H */
