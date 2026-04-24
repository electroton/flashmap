#include "flashgroup_report.h"
#include <stdio.h>

void flashgroup_report_summary(const FlashGroup *group, FILE *out) {
    if (!group || !out) return;
    uint32_t total = flashgroup_total_size(group);
    fprintf(out, "[GROUP] %s: %zu region(s), %u bytes used\n",
            group->name, group->count, total);
}

void flashgroup_report_detail(const FlashGroup *group, FILE *out) {
    if (!group || !out) return;

    uint32_t total = flashgroup_total_size(group);

    fprintf(out, "=== Group Report: %s ===\n", group->name);
    fprintf(out, "%-4s  %-24s  %-12s  %-10s  %s\n",
            "Idx", "Name", "Start", "Size", "% of Group");
    fprintf(out, "%-4s  %-24s  %-12s  %-10s  %s\n",
            "---", "------------------------",
            "------------", "----------", "----------");

    for (size_t i = 0; i < group->count; i++) {
        const FlashRegion *r = group->regions[i];
        double pct = (total > 0) ? (100.0 * r->size / total) : 0.0;
        fprintf(out, "%-4zu  %-24s  0x%08X    %-10u  %.1f%%\n",
                i, r->name, r->start, r->size, pct);
    }

    fprintf(out, "%-4s  %-24s  %-12s  %-10u  100.0%%\n",
            "", "TOTAL", "", total);
}
