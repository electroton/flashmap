#ifndef FLASHDIFF_H
#define FLASHDIFF_H

#include "flashlayout.h"
#include "flashregion.h"
#include <stddef.h>

/* Describes the type of change between two layouts */
typedef enum {
    DIFF_ADDED,
    DIFF_REMOVED,
    DIFF_MODIFIED,
    DIFF_UNCHANGED
} FlashDiffType;

/* Represents a single diff entry between two layouts */
typedef struct {
    FlashDiffType type;
    char name[64];
    uint32_t old_address;
    uint32_t old_size;
    uint32_t new_address;
    uint32_t new_size;
    int32_t  size_delta;
} FlashDiffEntry;

/* Result of comparing two flash layouts */
typedef struct {
    FlashDiffEntry *entries;
    size_t          count;
    size_t          capacity;
    int32_t         total_size_delta;
} FlashDiff;

/* Compare two layouts and populate a FlashDiff result.
 * Returns 0 on success, -1 on allocation failure. */
int flashdiff_compare(const FlashLayout *old_layout,
                      const FlashLayout *new_layout,
                      FlashDiff *out_diff);

/* Print a human-readable diff to stdout */
void flashdiff_print(const FlashDiff *diff);

/* Free resources held by a FlashDiff */
void flashdiff_free(FlashDiff *diff);

#endif /* FLASHDIFF_H */
