#include "flashmirror_report.h"
#include <stdio.h>

void flash_mirror_report(const FlashMirrorMap *map, FILE *out) {
    if (!map || !out) return;

    fprintf(out, "=== Flash Mirror Report ===\n");
    fprintf(out, "Total mirror pairs: %d\n\n", map->count);

    if (map->count == 0) {
        fprintf(out, "  No mirror pairs detected.\n");
        return;
    }

    fprintf(out, "%-4s  %-20s  %-12s  %-20s  %-12s  %-10s  %s\n",
            "#", "Primary", "Base", "Mirror", "Base", "Size", "Status");
    fprintf(out, "%-4s  %-20s  %-12s  %-20s  %-12s  %-10s  %s\n",
            "----", "--------------------", "------------",
            "--------------------", "------------", "----------", "------");

    for (int i = 0; i < map->count; i++) {
        const FlashMirrorPair *p = &map->pairs[i];
        fprintf(out, "%-4d  %-20s  0x%08X    %-20s  0x%08X    0x%08X  %s\n",
                i,
                p->primary_name, p->primary_base,
                p->mirror_name,  p->mirror_base,
                p->size,
                p->active ? "active" : "inactive");
    }
    fprintf(out, "\n");
}
