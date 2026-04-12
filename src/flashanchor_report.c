#include <stdio.h>
#include <string.h>
#include "flashanchor_report.h"

#define REPORT_BUF_SIZE 4096

static void print_header(FILE *f, size_t anchor_count, size_t region_count) {
    fprintf(f, "=== Flash Anchor Validation Report ===\n");
    fprintf(f, "Anchors : %zu\n", anchor_count);
    fprintf(f, "Regions : %zu\n", region_count);
    fprintf(f, "--------------------------------------\n");
}

static void print_anchor_table(FILE *f, const FlashAnchorSet *set) {
    fprintf(f, "\nDefined Anchors:\n");
    fprintf(f, "  %-32s %-12s %-10s %-10s %s\n",
            "Name", "Address", "Size", "Type", "Req");
    for (size_t i = 0; i < set->count; i++) {
        const FlashAnchor *a = &set->entries[i];
        fprintf(f, "  %-32s 0x%08X   %-10u %-10s %s\n",
                a->name, a->address, a->size,
                flash_anchor_type_str(a->type),
                a->required ? "yes" : "no");
    }
}

static int run_report(FILE *f, const FlashAnchorSet *set,
                      const FlashRegion *regions, size_t region_count) {
    char buf[REPORT_BUF_SIZE];
    print_header(f, set->count, region_count);
    print_anchor_table(f, set);

    fprintf(f, "\nValidation Results:\n");
    int violations = flash_anchor_validate(set, regions, region_count,
                                           buf, sizeof(buf));
    if (violations == 0) {
        fprintf(f, "  All anchors satisfied. No violations.\n");
    } else {
        fprintf(f, "%s", buf);
        fprintf(f, "\nTotal violations: %d\n", violations);
    }
    fprintf(f, "======================================\n");
    return violations;
}

int flash_anchor_report(const FlashAnchorSet *set,
                        const FlashRegion *regions, size_t region_count) {
    if (!set || !regions) return -1;
    return run_report(stdout, set, regions, region_count);
}

int flash_anchor_report_to_file(const FlashAnchorSet *set,
                                const FlashRegion *regions,
                                size_t region_count,
                                const char *filepath) {
    if (!set || !regions || !filepath) return -1;
    FILE *f = fopen(filepath, "w");
    if (!f) return -1;
    int result = run_report(f, set, regions, region_count);
    fclose(f);
    return result;
}
