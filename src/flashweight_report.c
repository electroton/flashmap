#include "flashweight_report.h"
#include <stdio.h>

void flash_weight_report_print(const FlashWeightResult *result, FILE *out) {
    if (!result || !out) return;

    fprintf(out, "%-24s  %10s  %10s  %8s  %8s\n",
            "Region", "Address", "Size", "Weight", "Pct(%)");
    fprintf(out, "%-24s  %10s  %10s  %8s  %8s\n",
            "------", "-------", "----", "------", "------");

    for (size_t i = 0; i < result->count; i++) {
        const FlashWeightEntry *e = &result->entries[i];
        fprintf(out, "%-24s  0x%08X  %10zu  %8.4f  %7.2f%%\n",
                e->name, e->address, e->size, e->weight, e->weight_pct);
    }

    fprintf(out, "\nTotal size : %zu bytes\n", result->total_size);
    fprintf(out, "Entropy    : %.4f bits\n", result->entropy);
}

void flash_weight_report_print_summary(const FlashWeightResult *result, FILE *out) {
    if (!result || !out) return;

    const FlashWeightEntry *heavy = flash_weight_heaviest(result);
    const FlashWeightEntry *light = flash_weight_lightest(result);

    fprintf(out, "Weight summary: %zu regions, total %zu bytes\n",
            result->count, result->total_size);
    if (heavy)
        fprintf(out, "  Heaviest : %s (%.2f%%)\n", heavy->name, heavy->weight_pct);
    if (light)
        fprintf(out, "  Lightest : %s (%.2f%%)\n", light->name, light->weight_pct);
    fprintf(out, "  Entropy  : %.4f bits\n", result->entropy);
}
