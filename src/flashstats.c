#include "flashstats.h"
#include <stdio.h>
#include <string.h>

int flashstats_compute(const FlashLayout *layout, FlashStats *out)
{
    if (!layout || !out || layout->count == 0)
        return -1;

    memset(out, 0, sizeof(*out));

    out->region_count   = layout->count;
    out->smallest_region = UINT32_MAX;
    out->min_address     = UINT32_MAX;

    for (size_t i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        uint32_t size = r->size;
        uint32_t end  = r->start + size;

        out->total_size += size;

        if (r->is_gap)
            out->free_size += size;
        else
            out->used_size += size;

        if (size > out->largest_region)
            out->largest_region = size;
        if (size < out->smallest_region)
            out->smallest_region = size;
        if (r->start < out->min_address)
            out->min_address = r->start;
        if (end > out->max_address)
            out->max_address = end;
    }

    out->avg_region_size  = (double)out->total_size / (double)out->region_count;
    out->address_span     = out->max_address - out->min_address;
    out->utilization_pct  = (out->total_size > 0)
                            ? (double)out->used_size / (double)out->total_size * 100.0
                            : 0.0;
    return 0;
}

void flashstats_print(const FlashStats *stats)
{
    if (!stats) return;
    printf("Flash Layout Statistics\n");
    printf("  Regions        : %zu\n",   stats->region_count);
    printf("  Address span   : 0x%08X - 0x%08X (%u bytes)\n",
           stats->min_address, stats->max_address, stats->address_span);
    printf("  Total size     : %u bytes\n", stats->total_size);
    printf("  Used           : %u bytes\n", stats->used_size);
    printf("  Free           : %u bytes\n", stats->free_size);
    printf("  Utilization    : %.1f%%\n",   stats->utilization_pct);
    printf("  Largest region : %u bytes\n", stats->largest_region);
    printf("  Smallest region: %u bytes\n", stats->smallest_region);
    printf("  Avg region size: %.1f bytes\n", stats->avg_region_size);
}

const FlashRegion *flashstats_busiest_region(const FlashLayout *layout)
{
    if (!layout || layout->count == 0) return NULL;

    const FlashRegion *best = NULL;
    double best_ratio = -1.0;

    for (size_t i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (r->is_gap || r->size == 0) continue;
        double ratio = (r->capacity > 0)
                       ? (double)r->size / (double)r->capacity
                       : 1.0;
        if (ratio > best_ratio) {
            best_ratio = ratio;
            best = r;
        }
    }
    return best;
}

void flashstats_utilization_bar(const FlashStats *stats, char *buf, int bar_width)
{
    if (!stats || !buf || bar_width <= 0) return;
    int filled = (int)(stats->utilization_pct / 100.0 * bar_width + 0.5);
    buf[0] = '[';
    for (int i = 0; i < bar_width; i++)
        buf[1 + i] = (i < filled) ? '#' : '.';
    buf[1 + bar_width] = ']';
    buf[2 + bar_width] = '\0';
}
