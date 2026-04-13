#include "flashheatmap.h"
#include "flashregion.h"
#include <stdio.h>
#include <string.h>

int flashheatmap_build(FlashHeatmap *hm, const FlashLayout *layout, size_t cols) {
    if (!hm || !layout || cols == 0 || cols > HEATMAP_MAX_COLS) {
        return -1;
    }
    if (layout->region_count == 0) {
        return -1;
    }

    memset(hm, 0, sizeof(FlashHeatmap));
    hm->cols = cols;
    hm->rows = 1;

    /* Determine address range from layout */
    uint32_t min_addr = UINT32_MAX;
    uint32_t max_addr = 0;
    for (size_t i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (r->address < min_addr) min_addr = r->address;
        uint32_t end = r->address + r->size;
        if (end > max_addr) max_addr = end;
    }

    if (max_addr <= min_addr) return -1;

    hm->base_address = min_addr;
    hm->total_size   = max_addr - min_addr;

    uint32_t bucket_size = hm->total_size / (uint32_t)cols;
    if (bucket_size == 0) bucket_size = 1;

    /* Accumulate used bytes per bucket */
    uint32_t used[HEATMAP_MAX_COLS] = {0};

    for (size_t i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        uint32_t rstart = r->address - min_addr;
        uint32_t rend   = rstart + r->size;

        for (size_t c = 0; c < cols; c++) {
            uint32_t bstart = (uint32_t)c * bucket_size;
            uint32_t bend   = bstart + bucket_size;
            /* Overlap between [rstart,rend) and [bstart,bend) */
            uint32_t ostart = rstart > bstart ? rstart : bstart;
            uint32_t oend   = rend   < bend   ? rend   : bend;
            if (oend > ostart) {
                used[c] += oend - ostart;
            }
        }
    }

    /* Convert to density percentage */
    for (size_t c = 0; c < cols; c++) {
        uint32_t pct = (used[c] * 100u) / bucket_size;
        if (pct > 100) pct = 100;
        hm->density[0][c] = (uint8_t)pct;
    }

    return 0;
}

char flashheatmap_char(uint8_t density) {
    if (density == 0)         return ' ';
    if (density < HEATMAP_SPARSE)   return '.';
    if (density < HEATMAP_MODERATE) return '+';
    if (density < HEATMAP_DENSE)    return '#';
    return '@';
}

void flashheatmap_print(const FlashHeatmap *hm) {
    if (!hm) return;
    printf("Heatmap [0x%08X - 0x%08X] (%u bytes)\n",
           hm->base_address,
           hm->base_address + hm->total_size,
           hm->total_size);
    printf("|");
    for (size_t c = 0; c < hm->cols; c++) {
        putchar(flashheatmap_char(hm->density[0][c]));
    }
    printf("|\n");
    flashheatmap_print_legend();
}

void flashheatmap_print_legend(void) {
    printf("Legend: ' '=empty  '.'=<25%%  '+'=<50%%  '#'=<75%%  '@'=full\n");
}
