/**
 * flashboundary.h - Flash region boundary checking and enforcement
 *
 * Provides utilities for validating that regions stay within defined
 * flash memory boundaries and detecting boundary violations.
 */

#ifndef FLASHBOUNDARY_H
#define FLASHBOUNDARY_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#define FLASHBOUNDARY_MAX_BOUNDS 16

typedef enum {
    BOUNDARY_OK             = 0,
    BOUNDARY_OVERFLOW       = 1,  /* Region exceeds upper bound */
    BOUNDARY_UNDERFLOW      = 2,  /* Region starts below lower bound */
    BOUNDARY_MISALIGNED     = 3,  /* Region not aligned to boundary granularity */
    BOUNDARY_ZERO_SIZE      = 4   /* Region has zero size */
} BoundaryStatus;

typedef struct {
    uint32_t base;        /* Lower bound address */
    uint32_t limit;       /* Upper bound address (exclusive) */
    uint32_t granularity; /* Alignment granularity (0 = no check) */
    char     name[32];    /* Boundary zone name */
} FlashBoundary;

typedef struct {
    FlashBoundary bounds[FLASHBOUNDARY_MAX_BOUNDS];
    size_t        count;
} FlashBoundarySet;

typedef struct {
    const FlashRegion *region;
    BoundaryStatus     status;
    uint32_t           violation_addr;
} BoundaryViolation;

/* Initialize a boundary set */
void flashboundary_init(FlashBoundarySet *set);

/* Add a boundary zone to the set */
int flashboundary_add(FlashBoundarySet *set, uint32_t base, uint32_t limit,
                      uint32_t granularity, const char *name);

/* Check a single region against all boundaries in the set */
BoundaryStatus flashboundary_check(const FlashBoundarySet *set,
                                   const FlashRegion *region,
                                   BoundaryViolation *violation_out);

/* Check all regions in an array; returns number of violations found */
int flashboundary_check_all(const FlashBoundarySet *set,
                            const FlashRegion *regions, size_t count,
                            BoundaryViolation *violations_out, size_t max_violations);

/* Return a human-readable string for a BoundaryStatus */
const char *flashboundary_status_str(BoundaryStatus status);

#endif /* FLASHBOUNDARY_H */
