#ifndef FLASHRANGE_H
#define FLASHRANGE_H

#include <stdint.h>
#include <stdbool.h>
#include "flashregion.h"

/* Represents a contiguous address range within flash memory */
typedef struct {
    uint32_t start;   /* Inclusive start address */
    uint32_t end;     /* Inclusive end address */
    char     name[64];
} FlashRange;

/* Result of a range intersection or clip operation */
typedef struct {
    FlashRange range;
    bool       valid; /* false when the result is an empty/invalid range */
} FlashRangeResult;

/* Create a FlashRange from a start address and a byte size */
FlashRange flashrange_from_size(uint32_t start, uint32_t size, const char *name);

/* Create a FlashRange from a FlashRegion */
FlashRange flashrange_from_region(const FlashRegion *region);

/* Return true when addr falls within [range.start, range.end] */
bool flashrange_contains(const FlashRange *range, uint32_t addr);

/* Return true when a and b share at least one address */
bool flashrange_overlaps(const FlashRange *a, const FlashRange *b);

/* Compute the intersection of two ranges; result.valid is false if disjoint */
FlashRangeResult flashrange_intersect(const FlashRange *a, const FlashRange *b);

/* Clip range so that it fits entirely within boundary; result.valid is false
 * when range lies completely outside boundary */
FlashRangeResult flashrange_clip(const FlashRange *range,
                                  const FlashRange *boundary);

/* Return the byte size of a range (end - start + 1), or 0 if invalid */
uint32_t flashrange_size(const FlashRange *range);

/* Print a human-readable summary of the range to stdout */
void flashrange_print(const FlashRange *range);

#endif /* FLASHRANGE_H */
