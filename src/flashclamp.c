#include "flashclamp.h"
#include <string.h>

FlashClampResult flashclamp_region(const FlashRegion *region,
                                   const FlashClampBounds *bounds,
                                   FlashRegion *out)
{
    if (!region || !bounds || !out)
        return FLASHCLAMP_ERR_NULL;

    if (bounds->end < bounds->start)
        return FLASHCLAMP_ERR_INVALID_BOUNDS;

    uint32_t reg_end = region->address + (region->size > 0 ? region->size - 1 : 0);

    /* No overlap at all */
    if (region->address > bounds->end || reg_end < bounds->start)
        return FLASHCLAMP_ERR_NO_OVERLAP;

    /* Copy metadata then adjust address/size */
    *out = *region;

    uint32_t clamped_start = region->address < bounds->start
                             ? bounds->start : region->address;
    uint32_t clamped_end   = reg_end > bounds->end
                             ? bounds->end : reg_end;

    out->address = clamped_start;
    out->size    = clamped_end - clamped_start + 1;

    return FLASHCLAMP_OK;
}

FlashClampResult flashclamp_regions(const FlashRegion *regions,
                                    size_t count,
                                    const FlashClampBounds *bounds,
                                    FlashRegion *out,
                                    size_t *out_count)
{
    if (!regions || !bounds || !out || !out_count)
        return FLASHCLAMP_ERR_NULL;

    if (bounds->end < bounds->start)
        return FLASHCLAMP_ERR_INVALID_BOUNDS;

    *out_count = 0;

    for (size_t i = 0; i < count; i++) {
        FlashRegion clamped;
        FlashClampResult r = flashclamp_region(&regions[i], bounds, &clamped);
        if (r == FLASHCLAMP_OK) {
            out[(*out_count)++] = clamped;
        } else if (r != FLASHCLAMP_ERR_NO_OVERLAP) {
            return r;
        }
    }

    return FLASHCLAMP_OK;
}

const char *flashclamp_result_str(FlashClampResult result)
{
    switch (result) {
        case FLASHCLAMP_OK:              return "OK";
        case FLASHCLAMP_ERR_NULL:        return "Null pointer argument";
        case FLASHCLAMP_ERR_INVALID_BOUNDS: return "Invalid bounds (end < start)";
        case FLASHCLAMP_ERR_NO_OVERLAP:  return "No overlap with clamp window";
        default:                         return "Unknown error";
    }
}
