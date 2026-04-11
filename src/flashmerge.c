#include "flashmerge.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int region_cmp_by_start(const void *a, const void *b) {
    const FlashRegion *ra = (const FlashRegion *)a;
    const FlashRegion *rb = (const FlashRegion *)b;
    if (ra->start < rb->start) return -1;
    if (ra->start > rb->start) return  1;
    return 0;
}

FlashMergeResult flash_merge_layouts(const FlashLayout *base,
                                      const FlashLayout *overlay,
                                      FlashMergeStrategy strategy) {
    FlashMergeResult result;
    memset(&result, 0, sizeof(result));

    if (!base || !overlay) {
        result.error = FLASH_MERGE_ERR_NULL_INPUT;
        return result;
    }

    size_t total = base->region_count + overlay->region_count;
    result.merged = flash_layout_create();
    if (!result.merged) {
        result.error = FLASH_MERGE_ERR_ALLOC;
        return result;
    }

    FlashRegion *scratch = malloc(total * sizeof(FlashRegion));
    if (!scratch) {
        flash_layout_free(result.merged);
        result.merged = NULL;
        result.error = FLASH_MERGE_ERR_ALLOC;
        return result;
    }

    size_t n = 0;
    for (size_t i = 0; i < base->region_count; i++)
        scratch[n++] = base->regions[i];
    for (size_t i = 0; i < overlay->region_count; i++)
        scratch[n++] = overlay->regions[i];

    qsort(scratch, n, sizeof(FlashRegion), region_cmp_by_start);

    for (size_t i = 0; i < n; i++) {
        if (result.merged->region_count > 0) {
            FlashRegion *prev = &result.merged->regions[result.merged->region_count - 1];
            uint32_t prev_end = prev->start + prev->size;
            if (scratch[i].start < prev_end) {
                result.conflict_count++;
                if (strategy == FLASH_MERGE_STRATEGY_OVERLAY_WINS) {
                    /* keep scratch[i], discard overlap portion of prev */
                    if (scratch[i].start + scratch[i].size > prev_end) {
                        prev->size = scratch[i].start - prev->start;
                    } else {
                        /* fully shadowed: skip */
                        continue;
                    }
                } else if (strategy == FLASH_MERGE_STRATEGY_BASE_WINS) {
                    continue;
                }
                /* FLASH_MERGE_STRATEGY_KEEP_BOTH falls through */
            }
        }
        flash_layout_add_region(result.merged, &scratch[i]);
    }

    free(scratch);
    result.error = FLASH_MERGE_ERR_NONE;
    return result;
}

void flash_merge_result_free(FlashMergeResult *result) {
    if (!result) return;
    if (result->merged) {
        flash_layout_free(result->merged);
        result->merged = NULL;
    }
}

const char *flash_merge_error_str(FlashMergeError err) {
    switch (err) {
        case FLASH_MERGE_ERR_NONE:        return "none";
        case FLASH_MERGE_ERR_NULL_INPUT:  return "null input";
        case FLASH_MERGE_ERR_ALLOC:       return "allocation failure";
        default:                          return "unknown error";
    }
}
