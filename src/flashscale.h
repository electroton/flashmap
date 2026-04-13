#ifndef FLASHSCALE_H
#define FLASHSCALE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "flashregion.h"
#include "flashlayout.h"

/* Result of scaling a single region */
typedef struct {
    uint32_t original_size;
    uint32_t scaled_size;
    double   factor;
    bool     valid;
} FlashScaleResult;

/* Map of scaled results for an entire layout */
typedef struct {
    FlashScaleResult *results;
    size_t            count;
    double            factor;
    uint32_t          total_original;
    uint32_t          total_scaled;
    bool              valid;
} FlashScaleMap;

/**
 * Scale a single flash region by the given factor.
 * Returns a FlashScaleResult with valid=false on bad input.
 */
FlashScaleResult flash_scale_region(const FlashRegion *region, double factor);

/**
 * Scale all regions in a layout by the given factor.
 * Caller must free with flash_scale_map_free().
 */
FlashScaleMap flash_scale_layout(const FlashLayout *layout, double factor);

/**
 * Returns true if the total scaled size fits within flash_size bytes.
 */
bool flash_scale_fits(const FlashScaleMap *map, uint32_t flash_size);

/**
 * Free resources held by a FlashScaleMap.
 */
void flash_scale_map_free(FlashScaleMap *map);

#endif /* FLASHSCALE_H */
