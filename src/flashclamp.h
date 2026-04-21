#ifndef FLASHCLAMP_H
#define FLASHCLAMP_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/* Result codes for clamp operations */
typedef enum {
    FLASHCLAMP_OK = 0,
    FLASHCLAMP_ERR_NULL,
    FLASHCLAMP_ERR_INVALID_BOUNDS,
    FLASHCLAMP_ERR_NO_OVERLAP
} FlashClampResult;

/*
 * FlashClampBounds defines the window within which regions will be clamped.
 * Regions that fall entirely outside the window are excluded.
 * Regions that partially overlap are trimmed to fit.
 */
typedef struct {
    uint32_t start;   /* inclusive lower bound of the clamp window */
    uint32_t end;     /* inclusive upper bound of the clamp window */
} FlashClampBounds;

/*
 * Clamp a single FlashRegion to the given bounds.
 * On success, writes the clamped region into `out` and returns FLASHCLAMP_OK.
 * Returns FLASHCLAMP_ERR_NO_OVERLAP if the region lies entirely outside bounds.
 */
FlashClampResult flashclamp_region(const FlashRegion *region,
                                   const FlashClampBounds *bounds,
                                   FlashRegion *out);

/*
 * Clamp an array of FlashRegions to the given bounds.
 * `out` must point to a buffer of at least `count` FlashRegion elements.
 * Sets `*out_count` to the number of clamped regions written.
 * Returns FLASHCLAMP_OK on success.
 */
FlashClampResult flashclamp_regions(const FlashRegion *regions,
                                    size_t count,
                                    const FlashClampBounds *bounds,
                                    FlashRegion *out,
                                    size_t *out_count);

/* Return a human-readable string for a FlashClampResult code */
const char *flashclamp_result_str(FlashClampResult result);

#endif /* FLASHCLAMP_H */
