/*
 * flashindex_report.c
 * Report generation for flash index entries.
 */

#include <stdio.h>
#include <string.h>
#include "flashindex_report.h"
#include "flashindex.h"

void flashindex_report_print(const FlashIndex *index, FILE *out) {
    if (!index || !out) return;

    fprintf(out, "=== Flash Index Report ===\n");
    fprintf(out, "Total entries: %zu\n", index->count);

    if (index->count == 0) {
        fprintf(out, "(no entries)\n");
        return;
    }

    fprintf(out, "\n%-4s  %-24s  %-10s  %-10s  %-10s\n",
            "#", "Name", "Start", "End", "Size");
    fprintf(out, "%-4s  %-24s  %-10s  %-10s  %-10s\n",
            "----", "------------------------",
            "----------", "----------", "----------");

    for (size_t i = 0; i < index->count; i++) {
        const FlashIndexEntry *e = &index->entries[i];
        fprintf(out, "%-4zu  %-24s  0x%08X  0x%08X  0x%08X\n",
                i,
                e->name,
                e->start,
                e->start + e->size,
                e->size);
    }
    fprintf(out, "\n");
}

void flashindex_report_summary(const FlashIndex *index, FILE *out) {
    if (!index || !out) return;

    uint32_t total_size = 0;
    uint32_t min_start = UINT32_MAX;
    uint32_t max_end = 0;

    for (size_t i = 0; i < index->count; i++) {
        const FlashIndexEntry *e = &index->entries[i];
        total_size += e->size;
        if (e->start < min_start) min_start = e->start;
        uint32_t end = e->start + e->size;
        if (end > max_end) max_end = end;
    }

    fprintf(out, "=== Flash Index Summary ===\n");
    fprintf(out, "Entries     : %zu\n", index->count);

    if (index->count > 0) {
        fprintf(out, "Range start : 0x%08X\n", min_start);
        fprintf(out, "Range end   : 0x%08X\n", max_end);
        fprintf(out, "Total size  : 0x%08X (%u bytes)\n", total_size, total_size);
    }
    fprintf(out, "\n");
}
