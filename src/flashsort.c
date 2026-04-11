#include "flashsort.h"
#include "flashregion.h"
#include <stdlib.h>
#include <string.h>

void flash_sort_options_default(FlashSortOptions *opts)
{
    if (!opts) return;
    opts->key    = FLASH_SORT_BY_ADDRESS;
    opts->order  = FLASH_SORT_ASCENDING;
    opts->stable = 1;
}

/* Comparator context passed via a global (qsort has no user-data param) */
static FlashSortOptions g_sort_opts;

static int region_cmp(const void *a, const void *b)
{
    const FlashRegion *ra = (const FlashRegion *)a;
    const FlashRegion *rb = (const FlashRegion *)b;
    int result = 0;

    switch (g_sort_opts.key) {
        case FLASH_SORT_BY_ADDRESS:
            if (ra->address < rb->address)      result = -1;
            else if (ra->address > rb->address) result =  1;
            break;
        case FLASH_SORT_BY_SIZE:
            if (ra->size < rb->size)      result = -1;
            else if (ra->size > rb->size) result =  1;
            break;
        case FLASH_SORT_BY_NAME:
            result = strcmp(ra->name, rb->name);
            break;
        case FLASH_SORT_BY_USAGE:
            if (ra->used < rb->used)      result = -1;
            else if (ra->used > rb->used) result =  1;
            break;
        default:
            break;
    }

    if (g_sort_opts.order == FLASH_SORT_DESCENDING)
        result = -result;

    return result;
}

int flash_sort(FlashLayout *layout, const FlashSortOptions *opts)
{
    if (!layout || !opts) return -1;
    if (layout->region_count == 0) return 0;

    g_sort_opts = *opts;
    qsort(layout->regions,
          (size_t)layout->region_count,
          sizeof(FlashRegion),
          region_cmp);
    return 0;
}

FlashLayout *flash_sort_copy(const FlashLayout *layout, const FlashSortOptions *opts)
{
    if (!layout || !opts) return NULL;

    FlashLayout *copy = flash_layout_clone(layout);
    if (!copy) return NULL;

    if (flash_sort(copy, opts) != 0) {
        flash_layout_free(copy);
        return NULL;
    }
    return copy;
}
