#include "flashvisualizer.h"
#include <string.h>
#include <stdio.h>

void flashvis_print_bar(const FlashLayout *layout, FILE *stream) {
    if (!layout || !stream || layout->total_flash_size == 0) return;

    fprintf(stream, "Flash layout [0x%08X - 0x%08X] (%u bytes)\n",
            layout->base_address,
            layout->base_address + layout->total_flash_size,
            layout->total_flash_size);
    fprintf(stream, "|%-*s|\n", FLASHVIS_BAR_WIDTH, "");

    for (size_t i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        int filled = (int)((double)r->size / layout->total_flash_size * FLASHVIS_BAR_WIDTH);
        if (filled < 1 && r->size > 0) filled = 1;

        char bar[FLASHVIS_BAR_WIDTH + 1];
        memset(bar, '#', (size_t)filled);
        memset(bar + filled, '-', (size_t)(FLASHVIS_BAR_WIDTH - filled));
        bar[FLASHVIS_BAR_WIDTH] = '\0';

        fprintf(stream, "|%s| %-20s 0x%08X (%5u B)\n",
                bar, r->name, r->start_address, r->size);
    }
    fprintf(stream, "|%-*s|\n", FLASHVIS_BAR_WIDTH, "");
}

void flashvis_print_summary(const FlashLayout *layout, FILE *stream) {
    if (!layout || !stream) return;

    fprintf(stream, "%-20s  %-10s  %-10s  %s\n",
            "Region", "Start", "Size", "Usage");
    fprintf(stream, "%-20s  %-10s  %-10s  %s\n",
            "------", "-----", "----", "-----");

    for (size_t i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        double pct = 0.0;
        if (layout->total_flash_size > 0) {
            pct = (double)r->size / layout->total_flash_size * 100.0;
        }
        fprintf(stream, "%-20s  0x%08X  %10u  %5.1f%%\n",
                r->name, r->start_address, r->size, pct);
    }

    uint32_t used = flashlayout_used_bytes(layout);
    uint32_t free_b = flashlayout_free_bytes(layout);
    fprintf(stream, "\nTotal used : %u bytes\n", used);
    fprintf(stream, "Total free : %u bytes\n", free_b);
}
