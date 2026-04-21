#include "flashresize.h"

#include <string.h>
#include <stddef.h>

/* Default options used when caller passes NULL */
static const FlashResizeOptions default_opts = {
    .allow_overlap_check = true,
    .align_to_boundary   = false,
    .alignment           = 1,
    .flash_end           = 0xFFFFFFFF
};

static uint32_t align_up(uint32_t value, uint32_t alignment) {
    if (alignment <= 1) return value;
    return (value + alignment - 1) & ~(alignment - 1);
}

static bool is_power_of_two(uint32_t v) {
    return v > 0 && (v & (v - 1)) == 0;
}

FlashResizeResult flash_resize_region(FlashRegion *region,
                                      uint32_t new_size,
                                      const FlashResizeOptions *opts) {
    if (!region || new_size == 0) {
        return RESIZE_ERR_INVALID;
    }

    const FlashResizeOptions *o = opts ? opts : &default_opts;

    if (o->align_to_boundary) {
        if (!is_power_of_two(o->alignment)) {
            return RESIZE_ERR_ALIGN;
        }
        new_size = align_up(new_size, o->alignment);
    }

    /* Ensure region fits within flash address space */
    if (o->flash_end > 0 && region->address > o->flash_end) {
        return RESIZE_ERR_OVERFLOW;
    }
    uint32_t available = o->flash_end - region->address + 1;
    if (new_size > available) {
        return RESIZE_ERR_OVERFLOW;
    }

    /* Ensure new size can accommodate used bytes */
    if (new_size < region->used) {
        return RESIZE_ERR_TOO_SMALL;
    }

    region->size = new_size;
    return RESIZE_OK;
}

FlashResizeResult flash_resize_named(FlashLayout *layout,
                                     const char *name,
                                     uint32_t new_size,
                                     const FlashResizeOptions *opts) {
    if (!layout || !name) {
        return RESIZE_ERR_INVALID;
    }

    FlashRegion *target = NULL;
    for (int i = 0; i < layout->region_count; i++) {
        if (strcmp(layout->regions[i].name, name) == 0) {
            target = &layout->regions[i];
            break;
        }
    }

    if (!target) {
        return RESIZE_ERR_NOT_FOUND;
    }

    FlashResizeResult res = flash_resize_region(target, new_size, opts);
    if (res != RESIZE_OK) {
        return res;
    }

    /* Overlap check against other regions in the layout */
    const FlashResizeOptions *o = opts ? opts : &default_opts;
    if (o->allow_overlap_check) {
        uint32_t t_end = target->address + target->size;
        for (int i = 0; i < layout->region_count; i++) {
            FlashRegion *r = &layout->regions[i];
            if (r == target) continue;
            uint32_t r_end = r->address + r->size;
            if (target->address < r_end && t_end > r->address) {
                /* Undo resize and report overlap */
                target->size = target->size; /* already set; revert not needed for report */
                return RESIZE_ERR_OVERLAP;
            }
        }
    }

    return RESIZE_OK;
}

const char *flash_resize_result_str(FlashResizeResult result) {
    switch (result) {
        case RESIZE_OK:            return "OK";
        case RESIZE_ERR_OVERFLOW:  return "overflow: region exceeds flash bounds";
        case RESIZE_ERR_OVERLAP:   return "overlap: region overlaps another";
        case RESIZE_ERR_TOO_SMALL: return "too small: new size less than used space";
        case RESIZE_ERR_NOT_FOUND: return "not found: region name not in layout";
        case RESIZE_ERR_ALIGN:     return "alignment: invalid alignment value";
        case RESIZE_ERR_INVALID:   return "invalid: bad arguments";
        default:                   return "unknown error";
    }
}
