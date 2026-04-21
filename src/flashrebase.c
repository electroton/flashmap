/**
 * flashrebase.c - Rebase flash regions to a new base address
 */

#include "flashrebase.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashRebaseResult flash_rebase(const FlashLayout *layout,
                                uint32_t new_base,
                                FlashRebaseOptions opts,
                                FlashLayout *out)
{
    if (!layout || !out) {
        return FLASH_REBASE_ERR_NULL;
    }
    if (layout->count == 0) {
        out->count = 0;
        out->regions = NULL;
        return FLASH_REBASE_OK;
    }

    uint32_t old_base = layout->regions[0].start;
    for (size_t i = 1; i < layout->count; i++) {
        if (layout->regions[i].start < old_base)
            old_base = layout->regions[i].start;
    }

    int64_t delta = (int64_t)new_base - (int64_t)old_base;

    if (opts.check_overflow) {
        for (size_t i = 0; i < layout->count; i++) {
            int64_t new_end = (int64_t)layout->regions[i].start
                            + (int64_t)layout->regions[i].size
                            + delta;
            if (new_end > (int64_t)FLASH_REBASE_ADDR_MAX) {
                return FLASH_REBASE_ERR_OVERFLOW;
            }
            int64_t new_start = (int64_t)layout->regions[i].start + delta;
            if (new_start < 0) {
                return FLASH_REBASE_ERR_UNDERFLOW;
            }
        }
    }

    out->regions = malloc(layout->count * sizeof(FlashRegion));
    if (!out->regions) {
        return FLASH_REBASE_ERR_ALLOC;
    }
    out->count = layout->count;

    for (size_t i = 0; i < layout->count; i++) {
        out->regions[i] = layout->regions[i];
        out->regions[i].start = (uint32_t)((int64_t)layout->regions[i].start + delta);
        if (opts.copy_names && layout->regions[i].name) {
            out->regions[i].name = strdup(layout->regions[i].name);
        }
    }

    return FLASH_REBASE_OK;
}

const char *flash_rebase_result_str(FlashRebaseResult result)
{
    switch (result) {
        case FLASH_REBASE_OK:           return "OK";
        case FLASH_REBASE_ERR_NULL:     return "Null pointer argument";
        case FLASH_REBASE_ERR_OVERFLOW: return "Address overflow";
        case FLASH_REBASE_ERR_UNDERFLOW:return "Address underflow";
        case FLASH_REBASE_ERR_ALLOC:    return "Memory allocation failure";
        default:                        return "Unknown error";
    }
}

void flash_rebase_layout_free(FlashLayout *layout, bool free_names)
{
    if (!layout) return;
    if (free_names && layout->regions) {
        for (size_t i = 0; i < layout->count; i++) {
            free((char *)layout->regions[i].name);
        }
    }
    free(layout->regions);
    layout->regions = NULL;
    layout->count = 0;
}
