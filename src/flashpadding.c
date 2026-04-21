#include "flashpadding.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 8

static int padding_reserve(FlashPaddingResult *r) {
    if (r->count < r->capacity) return 1;
    size_t new_cap = r->capacity ? r->capacity * 2 : INITIAL_CAPACITY;
    FlashPaddingRegion *tmp = realloc(r->regions,
                                      new_cap * sizeof(FlashPaddingRegion));
    if (!tmp) return 0;
    r->regions  = tmp;
    r->capacity = new_cap;
    return 1;
}

FlashPaddingResult flash_padding_compute(const FlashRegion *regions,
                                          size_t count,
                                          uint32_t alignment,
                                          FlashPadFill fill,
                                          uint8_t pattern) {
    FlashPaddingResult result;
    memset(&result, 0, sizeof(result));

    if (!regions || count == 0 || alignment == 0)
        return result;

    for (size_t i = 0; i + 1 < count; i++) {
        uint32_t cur_end  = regions[i].start + regions[i].size;
        uint32_t next_start = regions[i + 1].start;

        /* Align cur_end up to the requested boundary */
        uint32_t aligned_end = (cur_end + alignment - 1) & ~(alignment - 1);

        if (aligned_end >= next_start)
            continue; /* no padding needed */

        if (!padding_reserve(&result)) break;

        FlashPaddingRegion *pr = &result.regions[result.count++];
        pr->start   = cur_end;
        pr->end     = aligned_end;
        pr->size    = aligned_end - cur_end;
        pr->fill    = fill;
        pr->pattern = pattern;

        result.total_padding += pr->size;
    }

    return result;
}

void flash_padding_result_free(FlashPaddingResult *result) {
    if (!result) return;
    free(result->regions);
    result->regions       = NULL;
    result->count         = 0;
    result->capacity      = 0;
    result->total_padding = 0;
}

size_t flash_padding_total(const FlashPaddingResult *result) {
    return result ? result->total_padding : 0;
}

void flash_padding_print(const FlashPaddingResult *result) {
    if (!result) return;
    printf("Padding regions: %zu  (total %zu bytes)\n",
           result->count, result->total_padding);
    for (size_t i = 0; i < result->count; i++) {
        const FlashPaddingRegion *pr = &result->regions[i];
        const char *fill_str =
            pr->fill == FLASH_PAD_ZERO    ? "0x00"    :
            pr->fill == FLASH_PAD_FF      ? "0xFF"    : "pattern";
        printf("  [%zu] 0x%08X - 0x%08X  size=%-6zu  fill=%s",
               i, pr->start, pr->end, pr->size, fill_str);
        if (pr->fill == FLASH_PAD_PATTERN)
            printf(" (0x%02X)", pr->pattern);
        putchar('\n');
    }
}
