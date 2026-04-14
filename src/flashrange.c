#include <stdio.h>
#include <string.h>
#include "flashrange.h"

FlashRange flashrange_from_size(uint32_t start, uint32_t size,
                                 const char *name)
{
    FlashRange r;
    r.start = start;
    r.end   = (size > 0) ? (start + size - 1) : start;
    strncpy(r.name, name ? name : "", sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';
    return r;
}

FlashRange flashrange_from_region(const FlashRegion *region)
{
    if (!region) {
        FlashRange empty = {0, 0, ""};
        return empty;
    }
    return flashrange_from_size(region->start, region->size, region->name);
}

bool flashrange_contains(const FlashRange *range, uint32_t addr)
{
    if (!range) return false;
    return addr >= range->start && addr <= range->end;
}

bool flashrange_overlaps(const FlashRange *a, const FlashRange *b)
{
    if (!a || !b) return false;
    return a->start <= b->end && b->start <= a->end;
}

FlashRangeResult flashrange_intersect(const FlashRange *a,
                                       const FlashRange *b)
{
    FlashRangeResult res = {{0, 0, ""}, false};
    if (!a || !b) return res;
    if (!flashrange_overlaps(a, b)) return res;

    uint32_t s = (a->start > b->start) ? a->start : b->start;
    uint32_t e = (a->end   < b->end)   ? a->end   : b->end;
    snprintf(res.range.name, sizeof(res.range.name),
             "%s^%s", a->name, b->name);
    res.range.start = s;
    res.range.end   = e;
    res.valid = true;
    return res;
}

FlashRangeResult flashrange_clip(const FlashRange *range,
                                  const FlashRange *boundary)
{
    return flashrange_intersect(range, boundary);
}

uint32_t flashrange_size(const FlashRange *range)
{
    if (!range || range->end < range->start) return 0;
    return range->end - range->start + 1;
}

void flashrange_print(const FlashRange *range)
{
    if (!range) {
        printf("FlashRange: (null)\n");
        return;
    }
    printf("FlashRange [%s]: 0x%08X - 0x%08X (%u bytes)\n",
           range->name, range->start, range->end,
           flashrange_size(range));
}
