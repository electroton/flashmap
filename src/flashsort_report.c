#include "flashsort.h"
#include "flashregion.h"
#include "flashlayout.h"
#include <stdio.h>
#include <string.h>

static const char *sort_key_name(FlashSortKey key)
{
    switch (key) {
        case FLASH_SORT_BY_ADDRESS: return "address";
        case FLASH_SORT_BY_SIZE:    return "size";
        case FLASH_SORT_BY_NAME:    return "name";
        case FLASH_SORT_BY_USAGE:   return "usage";
        default:                    return "unknown";
    }
}

static const char *sort_order_name(FlashSortOrder order)
{
    return (order == FLASH_SORT_ASCENDING) ? "ascending" : "descending";
}

/*
 * Print a sorted region table to the given file stream.
 * Sorts a copy of the layout and prints the result.
 */
void flash_sort_print_report(FILE *out, const FlashLayout *layout, const FlashSortOptions *opts)
{
    if (!out || !layout || !opts) return;

    FlashLayout *sorted = flash_sort_copy(layout, opts);
    if (!sorted) {
        fprintf(out, "[flashsort] Error: failed to sort layout.\n");
        return;
    }

    fprintf(out, "\n=== Flash Region Sort Report ===\n");
    fprintf(out, "Sorted by: %s (%s)\n\n",
            sort_key_name(opts->key),
            sort_order_name(opts->order));
    fprintf(out, "%-20s  %10s  %10s  %10s  %6s\n",
            "Name", "Address", "Size", "Used", "Use%");
    fprintf(out, "%-20s  %10s  %10s  %10s  %6s\n",
            "--------------------",
            "----------", "----------", "----------", "------");

    for (int i = 0; i < sorted->region_count; i++) {
        const FlashRegion *r = &sorted->regions[i];
        double pct = (r->size > 0) ? (100.0 * r->used / r->size) : 0.0;
        fprintf(out, "%-20s  0x%08X  0x%08X  0x%08X  %5.1f%%\n",
                r->name, r->address, r->size, r->used, pct);
    }

    fprintf(out, "\nTotal regions: %d\n", sorted->region_count);
    flash_layout_free(sorted);
}
