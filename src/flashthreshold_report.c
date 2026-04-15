#include "flashthreshold_report.h"
#include <stdio.h>

void flash_threshold_report_print(const FlashThresholdResult *results,
                                  int count,
                                  FILE *out) {
    if (!results || count <= 0 || !out) return;

    fprintf(out, "%-20s %10s %10s %7s  %s\n",
            "Region", "Used", "Total", "Use%", "Status");
    fprintf(out, "%-20s %10s %10s %7s  %s\n",
            "------", "----", "-----", "----", "------");

    for (int i = 0; i < count; i++) {
        const FlashThresholdResult *r = &results[i];
        fprintf(out, "%-20s %10u %10u %6u%%  %s\n",
                r->region_name,
                r->used_bytes,
                r->total_bytes,
                r->used_percent,
                flash_threshold_level_str(r->level));
    }
}
