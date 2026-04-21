#include "flashpadding_report.h"
#include <stdio.h>

void flash_padding_report_csv(const FlashPaddingResult *result, FILE *out) {
    if (!result || !out) return;
    fprintf(out, "index,start,end,size,fill,pattern\n");
    for (size_t i = 0; i < result->count; i++) {
        const FlashPaddingRegion *pr = &result->regions[i];
        const char *fill_str =
            pr->fill == FLASH_PAD_ZERO    ? "zero"    :
            pr->fill == FLASH_PAD_FF      ? "0xff"    : "pattern";
        fprintf(out, "%zu,0x%08X,0x%08X,%zu,%s,0x%02X\n",
                i, pr->start, pr->end, pr->size, fill_str, pr->pattern);
    }
}

void flash_padding_report_text(const FlashPaddingResult *result, FILE *out) {
    if (!result || !out) return;
    fprintf(out, "=== Flash Padding Report ===\n");
    fprintf(out, "Regions : %zu\n", result->count);
    fprintf(out, "Total   : %zu bytes\n\n", result->total_padding);
    for (size_t i = 0; i < result->count; i++) {
        const FlashPaddingRegion *pr = &result->regions[i];
        const char *fill_str =
            pr->fill == FLASH_PAD_ZERO    ? "zero"    :
            pr->fill == FLASH_PAD_FF      ? "0xFF"    : "pattern";
        fprintf(out, "  #%zu  start=0x%08X  end=0x%08X  size=%zu  fill=%s",
                i, pr->start, pr->end, pr->size, fill_str);
        if (pr->fill == FLASH_PAD_PATTERN)
            fprintf(out, " (0x%02X)", pr->pattern);
        fputc('\n', out);
    }
}
