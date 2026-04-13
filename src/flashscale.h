/**
 * flashscale.h - Flash region scaling and normalization utilities
 *
 * Provides functions to scale flash region sizes and addresses
 * relative to a reference total capacity, useful for proportional
 * visualization and percentage-based reporting.
 */

#ifndef FLASHSCALE_H
#define FLASHSCALE_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"
#include "flashlayout.h"

/**
 * Scaled representation of a flash region.
 */
typedef struct {
    const FlashRegion *region;   /* Pointer to original region */
    double offset_pct;           /* Start offset as % of total capacity */
    double size_pct;             /* Size as % of total capacity */
    double used_pct;             /* Used bytes as % of region size */
} FlashScaledRegion;

/**
 * Result of a layout scaling operation.
 */
typedef struct {
    FlashScaledRegion *entries;  /* Array of scaled regions */
    size_t count;                /* Number of entries */
    uint32_t total_capacity;     /* Reference total capacity in bytes */
    double overall_used_pct;     /* Overall used percentage across layout */
} FlashScaleResult;

/**
 * Scale all regions in a layout relative to its total capacity.
 *
 * @param layout   Source flash layout
 * @param result   Output scale result (caller must call flashscale_free)
 * @return 0 on success, -1 on error
 */
int flashscale_layout(const FlashLayout *layout, FlashScaleResult *result);

/**
 * Scale a single region relative to a given total capacity.
 *
 * @param region         Source region
 * @param total_capacity Reference capacity in bytes
 * @param out            Output scaled region
 * @return 0 on success, -1 on error
 */
int flashscale_region(const FlashRegion *region, uint32_t total_capacity,
                      FlashScaledRegion *out);

/**
 * Print a human-readable scale report to stdout.
 *
 * @param result Populated FlashScaleResult
 */
void flashscale_print(const FlashScaleResult *result);

/**
 * Free resources allocated by flashscale_layout.
 *
 * @param result Scale result to free
 */
void flashscale_free(FlashScaleResult *result);

#endif /* FLASHSCALE_H */
