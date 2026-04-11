/**
 * flashmerge.h - Flash region merging utilities
 *
 * Provides functionality to merge adjacent or overlapping flash regions
 * within a flash layout into consolidated regions.
 */

#ifndef FLASHMERGE_H
#define FLASHMERGE_H

#include "flashlayout.h"
#include "flashregion.h"
#include <stdint.h>

/**
 * Merge strategy for combining regions.
 */
typedef enum {
    MERGE_ADJACENT,    /* Merge only directly adjacent regions (no gap) */
    MERGE_OVERLAPPING, /* Merge only overlapping regions */
    MERGE_BOTH         /* Merge both adjacent and overlapping regions */
} MergeStrategy;

/**
 * Result of a merge operation.
 */
typedef struct {
    FlashLayout *layout;  /* Resulting merged layout (caller must free) */
    int merged_count;     /* Number of merge operations performed */
} MergeResult;

/**
 * Merge regions in the given layout according to the specified strategy.
 *
 * @param layout    Source layout to merge regions from
 * @param strategy  Merging strategy to apply
 * @return          MergeResult with new layout; layout is NULL on failure
 */
MergeResult flashmerge_apply(const FlashLayout *layout, MergeStrategy strategy);

/**
 * Check whether two regions can be merged under the given strategy.
 *
 * @param a         First region
 * @param b         Second region
 * @param strategy  Merging strategy
 * @return          1 if regions can be merged, 0 otherwise
 */
int flashmerge_can_merge(const FlashRegion *a, const FlashRegion *b, MergeStrategy strategy);

/**
 * Produce a single merged region from two compatible regions.
 * The resulting region spans from min(a->start, b->start) to
 * max(a->end, b->end). The name is taken from region a.
 *
 * @param a  First region
 * @param b  Second region
 * @param out Output merged region
 */
void flashmerge_combine(const FlashRegion *a, const FlashRegion *b, FlashRegion *out);

#endif /* FLASHMERGE_H */
