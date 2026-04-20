#include "flashmark_report.h"
#include <stdio.h>

static const char *type_str(FlashMarkType t)
{
    switch (t) {
    case FLASHMARK_TYPE_START:  return "START";
    case FLASHMARK_TYPE_END:    return "END";
    case FLASHMARK_TYPE_CUSTOM: return "CUSTOM";
    default:                    return "UNKNOWN";
    }
}

void flashmark_report_print(const FlashMarkSet *set, FILE *stream)
{
    if (!set || !stream) return;

    fprintf(stream, "%-40s  %-10s  %-8s  %s\n",
            "Name", "Address", "Type", "Region");
    fprintf(stream, "%-40s  %-10s  %-8s  %s\n",
            "----------------------------------------",
            "----------", "--------", "------");

    for (size_t i = 0; i < set->count; i++) {
        const FlashMark *m = &set->marks[i];
        if (m->region_index >= 0)
            fprintf(stream, "%-40s  0x%08X  %-8s  %d\n",
                    m->name, m->address, type_str(m->type), m->region_index);
        else
            fprintf(stream, "%-40s  0x%08X  %-8s  -\n",
                    m->name, m->address, type_str(m->type));
    }
    fprintf(stream, "Total marks: %zu\n", set->count);
}

void flashmark_report_by_type(const FlashMarkSet *set, FlashMarkType type,
                               FILE *stream)
{
    if (!set || !stream) return;

    fprintf(stream, "Marks of type [%s]:\n", type_str(type));
    size_t shown = 0;
    for (size_t i = 0; i < set->count; i++) {
        const FlashMark *m = &set->marks[i];
        if (m->type == type) {
            fprintf(stream, "  %-40s  0x%08X\n", m->name, m->address);
            shown++;
        }
    }
    if (shown == 0)
        fprintf(stream, "  (none)\n");
}
