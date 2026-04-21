#include "flashoffset.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashOffsetResult flash_offset_apply(const FlashRegion *region, uint32_t delta, FlashOffsetMode mode) {
    FlashOffsetResult result = {0};
    if (!region) {
        result.status = FLASH_OFFSET_ERR_NULL;
        return result;
    }

    result.original_start = region->start;
    result.original_end   = region->start + region->size;

    switch (mode) {
        case FLASH_OFFSET_ADD:
            result.new_start = region->start + delta;
            break;
        case FLASH_OFFSET_SUB:
            if (delta > region->start) {
                result.status = FLASH_OFFSET_ERR_UNDERFLOW;
                return result;
            }
            result.new_start = region->start - delta;
            break;
        case FLASH_OFFSET_ABS:
            result.new_start = delta;
            break;
        default:
            result.status = FLASH_OFFSET_ERR_INVALID_MODE;
            return result;
    }

    result.new_end = result.new_start + region->size;
    result.delta   = (int64_t)result.new_start - (int64_t)result.original_start;
    result.status  = FLASH_OFFSET_OK;
    return result;
}

int flash_offset_apply_layout(FlashLayout *layout, uint32_t delta, FlashOffsetMode mode) {
    if (!layout) return FLASH_OFFSET_ERR_NULL;

    for (size_t i = 0; i < layout->count; i++) {
        FlashOffsetResult r = flash_offset_apply(&layout->regions[i], delta, mode);
        if (r.status != FLASH_OFFSET_OK) return r.status;
        layout->regions[i].start = r.new_start;
    }
    return FLASH_OFFSET_OK;
}

bool flash_offset_would_overlap(const FlashOffsetResult *a, const FlashOffsetResult *b) {
    if (!a || !b) return false;
    return a->new_start < b->new_end && b->new_start < a->new_end;
}

const char *flash_offset_status_str(int status) {
    switch (status) {
        case FLASH_OFFSET_OK:               return "OK";
        case FLASH_OFFSET_ERR_NULL:         return "Null pointer";
        case FLASH_OFFSET_ERR_UNDERFLOW:    return "Underflow: delta exceeds region start";
        case FLASH_OFFSET_ERR_INVALID_MODE: return "Invalid offset mode";
        default:                            return "Unknown error";
    }
}
