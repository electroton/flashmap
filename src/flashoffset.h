/**
 * flashoffset.h - Flash region offset calculation and adjustment utilities
 *
 * Provides functions for computing, shifting, and normalizing address offsets
 * within flash regions relative to a given base address.
 */

#ifndef FLASHOFFSET_H
#define FLASHOFFSET_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/**
 * Result codes for offset operations.
 */
typedef enum {
    FLASHOFFSET_OK            = 0,
    FLASHOFFSET_ERR_NULL      = -1,
    FLASHOFFSET_ERR_UNDERFLOW = -2,
    FLASHOFFSET_ERR_OVERFLOW  = -3
} FlashOffsetResult;

/**
 * Represents a region expressed as an offset from a base address.
 */
typedef struct {
    const char *name;
    uint32_t    base;    /**< Base address used for offset calculation */
    uint32_t    offset;  /**< Offset of region start from base */
    uint32_t    size;    /**< Size of the region in bytes */
} FlashOffsetEntry;

/**
 * Compute the offset of a region relative to the given base address.
 *
 * @param region  Source flash region.
 * @param base    Base address to subtract.
 * @param out     Output entry populated on success.
 * @return FLASHOFFSET_OK on success, error code otherwise.
 */
FlashOffsetResult flashoffset_compute(const FlashRegion *region,
                                      uint32_t base,
                                      FlashOffsetEntry *out);

/**
 * Shift all regions in an array by a signed delta, updating their addresses.
 *
 * @param regions   Array of FlashRegion structs to modify in-place.
 * @param count     Number of regions.
 * @param delta     Signed byte delta to apply to each region's start address.
 * @return FLASHOFFSET_OK on success, error code if any region would underflow.
 */
FlashOffsetResult flashoffset_shift(FlashRegion *regions,
                                    size_t count,
                                    int32_t delta);

/**
 * Normalize an array of regions so the lowest address becomes the new base (0).
 *
 * @param regions   Array of FlashRegion structs to normalize in-place.
 * @param count     Number of regions.
 * @return FLASHOFFSET_OK on success, FLASHOFFSET_ERR_NULL if regions is NULL.
 */
FlashOffsetResult flashoffset_normalize(FlashRegion *regions, size_t count);

#endif /* FLASHOFFSET_H */
