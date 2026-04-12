#include "flashlabel_report.h"
#include <stdio.h>

static const char *kind_str(FlashLabelKind k) {
    switch (k) {
        case FLASHLABEL_KIND_SECTION:  return "section";
        case FLASHLABEL_KIND_SYMBOL:   return "symbol";
        case FLASHLABEL_KIND_BOUNDARY: return "boundary";
        case FLASHLABEL_KIND_CUSTOM:   return "custom";
        default:                       return "unknown";
    }
}

static void print_header(FILE *out) {
    fprintf(out, "%-24s  %-10s  %-8s  %-9s  %-6s  %s\n",
            "Name", "Address", "Size", "Kind", "Pinned", "Note");
    fprintf(out, "%-24s  %-10s  %-8s  %-9s  %-6s  %s\n",
            "------------------------",
            "----------", "--------",
            "---------", "------", "----");
}

void flashlabel_report_print(const FlashLabelSet *set, FILE *out) {
    if (!set || !out) return;
    print_header(out);
    for (size_t i = 0; i < set->count; i++) {
        const FlashLabel *l = &set->entries[i];
        fprintf(out, "%-24s  0x%08X  %8u  %-9s  %-6s  %s\n",
                l->name,
                l->address,
                l->size,
                kind_str(l->kind),
                l->pinned ? "yes" : "no",
                l->note[0] ? l->note : "-");
    }
    fprintf(out, "\nTotal labels: %zu\n", set->count);
}

void flashlabel_report_by_kind(const FlashLabelSet *set,
                                FlashLabelKind kind, FILE *out) {
    if (!set || !out) return;
    print_header(out);
    size_t shown = 0;
    for (size_t i = 0; i < set->count; i++) {
        const FlashLabel *l = &set->entries[i];
        if (l->kind != kind) continue;
        fprintf(out, "%-24s  0x%08X  %8u  %-9s  %-6s  %s\n",
                l->name,
                l->address,
                l->size,
                kind_str(l->kind),
                l->pinned ? "yes" : "no",
                l->note[0] ? l->note : "-");
        shown++;
    }
    fprintf(out, "\nShowing %zu of %zu labels (kind=%s)\n",
            shown, set->count, kind_str(kind));
}
