#include "flashencode_report.h"
#include <stdio.h>
#include <string.h>

/* Base64 of 64 bytes => ~88 chars; hex of 64 bytes => 128 chars; +1 for NUL */
#define ENCODE_BUF_SIZE 256

void flash_encode_report_layout(const FlashLayout *layout,
                                 FlashEncodeFormat fmt,
                                 FILE *fp) {
    if (!layout || !fp) return;

    const char *fmt_name = (fmt == FLASH_ENCODE_FMT_BASE64) ? "base64" : "hex";
    fprintf(fp, "=== Flash Encode Report [%s] ===\n", fmt_name);
    fprintf(fp, "%-20s  %s\n", "Region", "Encoded");
    fprintf(fp, "%-20s  %s\n", "------", "-------");

    char buf[ENCODE_BUF_SIZE];
    for (int i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        FlashEncodeResult res = flash_encode_region(r, fmt, buf, sizeof(buf));
        if (res == FLASH_ENCODE_OK) {
            fprintf(fp, "%-20s  %s\n", r->name, buf);
        } else {
            fprintf(fp, "%-20s  [error: %s]\n", r->name,
                    flash_encode_result_str(res));
        }
    }
    fprintf(fp, "Total regions: %d\n", layout->count);
}
