/**
 * flashsplit.c - Implementation of flash region splitting
 */

#include "flashsplit.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashSplitResult flashsplit_by_size(const FlashRegion *region, uint32_t chunk_size)
{
    FlashSplitResult result = { NULL, 0 };

    if (!region || chunk_size == 0) {
        return result;
    }

    uint32_t total = region->size;
    if (total == 0) {
        return result;
    }

    size_t count = (total + chunk_size - 1) / chunk_size;
    FlashRegion *regions = calloc(count, sizeof(FlashRegion));
    if (!regions) {
        return result;
    }

    uint32_t offset = 0;
    for (size_t i = 0; i < count; i++) {
        uint32_t sz = (offset + chunk_size <= total) ? chunk_size : (total - offset);
        regions[i].start   = region->start + offset;
        regions[i].size    = sz;
        regions[i].end     = regions[i].start + sz - 1;
        regions[i].type    = region->type;
        snprintf(regions[i].name, sizeof(regions[i].name),
                 "%s_part%zu", region->name, i + 1);
        offset += sz;
    }

    result.regions = regions;
    result.count   = count;
    return result;
}

int flashsplit_at_address(const FlashRegion *region, uint32_t address,
                          FlashRegion *lower, FlashRegion *upper)
{
    if (!region || !lower || !upper) {
        return -1;
    }

    if (address <= region->start || address > region->end) {
        return -1;
    }

    /* Lower region: [start, address) */
    *lower = *region;
    lower->end  = address - 1;
    lower->size = address - region->start;
    snprintf(lower->name, sizeof(lower->name), "%s_lo", region->name);

    /* Upper region: [address, end] */
    *upper = *region;
    upper->start = address;
    upper->size  = region->end - address + 1;
    snprintf(upper->name, sizeof(upper->name), "%s_hi", region->name);

    return 0;
}

void flashsplit_result_free(FlashSplitResult *result)
{
    if (!result) {
        return;
    }
    free(result->regions);
    result->regions = NULL;
    result->count   = 0;
}
