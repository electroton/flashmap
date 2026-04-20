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
 * @param chunk_size Size of each chunk in bytes (must be > 0 and must evenly
 *                   divide the region size; otherwise returns {NULL, 0})
 * @return           FlashSplitResult with allocated regions, or {NULL, 0} on error
 *                   Caller must free result.regions with flashsplit_result_free()
 */
FlashSplitResult flashsplit_by_size(const FlashRegion *region, uint32_t chunk_size);

/**
 * Split a region at a specific address boundary.
 *
 * @param region   Source region to split
 * @param address  Address at which to split (must be strictly within region,
 *                 i.e. region->start < address < region->end)
 * @param lower    Output: lower sub-region [region->start, address)
 * @param upper    Output: upper sub-region [address, region->end]
 * @return         0 on success, -1 if address is out of range or equals
 *                 region->start / region->end (degenerate split)
 */
int flashsplit_at_address(const FlashRegion *region, uint32_t address,
                          FlashRegion *lower, FlashRegion *upper);

/**
 * Split a region into N equal parts.
 *
 * Convenience wrapper around flashsplit_by_size() that computes the chunk
 * size from the region length and the requested part count.
 *
 * @param region  Source region to split
 * @param n       Number of parts (must be > 0 and must evenly divide the
 *                region size; otherwise returns {NULL, 0})
 * @return        FlashSplitResult with allocated regions, or {NULL, 0} on error
 *                Caller must free result.regions with flashsplit_result_free()
 */
FlashSplitResult flashsplit_into_n(const FlashRegion *region, size_t n);

/**
 * Free resources held by a FlashSplitResult.
 *
 * Safe to call with a NULL pointer or a result whose regions field is NULL.
 *
 * @param result  Pointer to result to free
 */
void flashsplit_result_free(FlashSplitResult *result);

#endif /* FLASHSPLIT_H */
