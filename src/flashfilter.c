#include "flashfilter.h"
#include "flashlayout.h"
#include "flashregion.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void flashfilter_init(FlashFilter *filter)
{
    if (!filter) return;
    filter->flags        = 0;
    filter->name_pattern = NULL;
    filter->min_size     = 0;
    filter->max_size     = UINT32_MAX;
    filter->min_usage_pct = 0.0f;
    filter->max_usage_pct = 100.0f;
}

static int region_matches(const FlashRegion *region, const FlashFilter *filter)
{
    if (filter->flags & FILTER_BY_NAME) {
        if (!filter->name_pattern) return 0;
        if (strstr(region->name, filter->name_pattern) == NULL)
            return 0;
    }

    if (filter->flags & FILTER_BY_MIN_SIZE) {
        if (region->size < filter->min_size)
            return 0;
    }

    if (filter->flags & FILTER_BY_MAX_SIZE) {
        if (region->size > filter->max_size)
            return 0;
    }

    if (filter->flags & FILTER_BY_USAGE) {
        if (region->size == 0) return 0;
        float pct = 100.0f * (float)region->used / (float)region->size;
        if (pct < filter->min_usage_pct || pct > filter->max_usage_pct)
            return 0;
    }

    return 1;
}

FlashLayout *flashfilter_apply(const FlashLayout *layout,
                               const FlashFilter *filter)
{
    if (!layout || !filter) return NULL;

    FlashLayout *result = flashlayout_create();
    if (!result) return NULL;

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (region_matches(r, filter)) {
            if (flashlayout_add_region(result, r) != 0) {
                flashlayout_free(result);
                return NULL;
            }
        }
    }

    return result;
}
