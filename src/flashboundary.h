/*
 * flashboundary.h - Flash region boundary checking and enforcement
 */

#ifndef FLASHBOUNDARY_H
#define FLASHBOUNDARY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "flashregion.h"

/* Result codes for boundary checks */
typedef enum {
    FLASH_BOUNDARY_OK           = 0,
    FLASH_BOUNDARY_OVERFLOW     = 1,  /* access extends past region end */
    FLASH_BOUNDARY_UNDERFLOW    = 2,  /* access starts before region start */
    FLASH_BOUNDARY_ZERO_SIZE    = 3,  /* zero-length access */
    FLASH_BOUNDARY_INVALID_ARG  = 4
} FlashBoundaryResult;

/* Describes an overlap between two regions */
typedef struct {
    size_t   region_a_index;
    size_t   region_b_index;
    uint32_t overlap_start;
    uint32_t overlap_end;   /* exclusive */
} FlashBoundaryViolation;

/*
 * Check whether an access of `size` bytes starting at `address`
 * falls entirely within `region`.
 */
FlashBoundaryResult flash_boundary_check(const FlashRegion *region,
                                          uint32_t address,
                                          uint32_t size);

/* Returns true if `address` falls inside `region`. */
bool flash_boundary_contains(const FlashRegion *region, uint32_t address);

/* Returns true if regions a and b overlap in address space. */
bool flash_boundary_overlaps(const FlashRegion *a, const FlashRegion *b);

/*
 * Scan an array of regions for pairwise overlaps.
 * Caller must free the returned array with flash_boundary_free_violations().
 * Returns NULL if no violations found (out_count == 0).
 */
FlashBoundaryViolation *flash_boundary_find_violations(
        const FlashRegion *regions, size_t count, size_t *out_count);

void flash_boundary_free_violations(FlashBoundaryViolation *violations);

/* Human-readable string for a result code. */
const char *flash_boundary_result_str(FlashBoundaryResult result);

#endif /* FLASHBOUNDARY_H */
