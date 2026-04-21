#include <stdlib.h>
#include <string.h>
#include "flashcrop.h"

static int region_in_window(const FlashRegion *r,
                            uint32_t win_start, uint32_t win_end)
{
    uint32_t r_end = r->address + r->size;
    return r->address < win_end && r_end > win_start;
}

static int region_fully_inside(const FlashRegion *r,
                               uint32_t win_start, uint32_t win_end)
{
    uint32_t r_end = r->address + r->size;
    return r->address >= win_start && r_end <= win_end;
}

int flash_crop(const FlashLayout *layout,
               uint32_t start, uint32_t size,
               FlashCropMode mode,
               FlashCropResult *result)
{
    if (!layout || !result || size == 0)
        return -1;

    uint32_t win_end = start + size;

    result->layout   = flash_layout_create();
    result->excluded = 0;
    result->trimmed  = 0;

    if (!result->layout)
        return -1;

    for (size_t i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];

        if (!region_in_window(r, start, win_end)) {
            result->excluded++;
            continue;
        }

        if (region_fully_inside(r, start, win_end)) {
            if (flash_layout_add_region(result->layout, r) != 0) {
                flash_layout_free(result->layout);
                result->layout = NULL;
                return -1;
            }
            continue;
        }

        /* Partially overlapping region */
        if (mode == CROP_MODE_EXCLUDE) {
            result->excluded++;
            continue;
        }

        /* CROP_MODE_TRIM: clamp region to window */
        FlashRegion trimmed_r = *r;
        uint32_t r_end = r->address + r->size;

        if (trimmed_r.address < start)
            trimmed_r.address = start;

        uint32_t new_end = (r_end < win_end) ? r_end : win_end;
        trimmed_r.size   = new_end - trimmed_r.address;

        if (flash_layout_add_region(result->layout, &trimmed_r) != 0) {
            flash_layout_free(result->layout);
            result->layout = NULL;
            return -1;
        }
        result->trimmed++;
    }

    return 0;
}

void flash_crop_result_free(FlashCropResult *result)
{
    if (!result)
        return;
    if (result->layout) {
        flash_layout_free(result->layout);
        result->layout = NULL;
    }
    result->excluded = 0;
    result->trimmed  = 0;
}
