/*
 * flashwatch_report.c - Reporting for flash watch results
 */

#include "flashwatch_report.h"
#include <stdio.h>

void flashwatch_report_print(const FlashWatch *watch, FILE *out) {
    if (!watch || !out) return;
    fprintf(out, "Flash Watch Report\n");
    fprintf(out, "==================\n");
    fprintf(out, "Watched regions: %zu\n\n", watch->count);
    for (size_t i = 0; i < watch->count; i++) {
        const FlashWatchEntry *e = &watch->entries[i];
        fprintf(out, "  [%zu] %-32s  base=0x%08X  size=0x%08X  prev_size=0x%08X  %s\n",
                i,
                e->region_name,
                e->base,
                e->size,
                e->prev_size,
                e->triggered ? "*** CHANGED ***" : "ok");
    }
    fprintf(out, "\n");
}

void flashwatch_report_triggered(const FlashWatch *watch, FILE *out) {
    if (!watch || !out) return;
    int any = 0;
    fprintf(out, "Triggered Watches\n");
    fprintf(out, "-----------------\n");
    for (size_t i = 0; i < watch->count; i++) {
        const FlashWatchEntry *e = &watch->entries[i];
        if (e->triggered) {
            fprintf(out, "  CHANGED: %-32s  base=0x%08X  size=0x%08X (was 0x%08X)\n",
                    e->region_name, e->base, e->size, e->prev_size);
            any++;
        }
    }
    if (!any)
        fprintf(out, "  (no changes detected)\n");
    fprintf(out, "\n");
}
