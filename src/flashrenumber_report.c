#include "flashrenumber_report.h"
#include <stdio.h>

void flash_renumber_report_print(const FlashRenumberResult *result, FILE *out) {
    if (!result || !out) return;

    if (result->status != FLASH_RENUMBER_OK) {
        fprintf(out, "[flashrenumber] Error: status=%d\n", result->status);
        return;
    }

    fprintf(out, "%-30s  %8s  %8s\n", "Region", "Old ID", "New ID");
    fprintf(out, "%-30s  %8s  %8s\n",
            "------------------------------", "--------", "--------");

    for (size_t i = 0; i < result->count; i++) {
        const FlashRenumberEntry *e = &result->entries[i];
        const char *name = (e->region && e->region->name[0])
                           ? e->region->name : "(unnamed)";
        fprintf(out, "%-30s  %8u  %8u\n", name, e->old_id, e->new_id);
    }
}

void flash_renumber_report_summary(const FlashRenumberResult *result, FILE *out) {
    if (!result || !out) return;

    if (result->status != FLASH_RENUMBER_OK) {
        fprintf(out, "[flashrenumber] renumbering failed (status=%d)\n",
                result->status);
        return;
    }

    fprintf(out, "[flashrenumber] %zu region(s) renumbered\n", result->count);
}
