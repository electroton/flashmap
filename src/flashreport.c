#include "flashreport.h"
#include "flashregion.h"
#include <stdio.h>
#include <string.h>

void report_config_init(ReportConfig *config) {
    if (!config) return;
    config->format = REPORT_FORMAT_TEXT;
    config->show_unused = 0;
    config->show_percent = 1;
}

static void report_text(const FlashLayout *layout, const ReportConfig *config, FILE *out) {
    fprintf(out, "%-24s %10s %10s %10s", "Region", "Start", "Size", "Used");
    if (config->show_percent) {
        fprintf(out, " %8s", "Usage%%");
    }
    fprintf(out, "\n");
    fprintf(out, "%-24s %10s %10s %10s", "------------------------",
            "----------", "----------", "----------");
    if (config->show_percent) {
        fprintf(out, " %8s", "--------");
    }
    fprintf(out, "\n");

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (!config->show_unused && r->used == 0) continue;

        fprintf(out, "%-24s 0x%08X %10u %10u",
                r->name, r->start_address, r->size, r->used);
        if (config->show_percent) {
            double pct = (r->size > 0) ? (100.0 * r->used / r->size) : 0.0;
            fprintf(out, " %7.1f%%", pct);
        }
        fprintf(out, "\n");
    }
}

static void report_csv(const FlashLayout *layout, const ReportConfig *config, FILE *out) {
    fprintf(out, "name,start,size,used");
    if (config->show_percent) fprintf(out, ",usage_pct");
    fprintf(out, "\n");

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (!config->show_unused && r->used == 0) continue;

        fprintf(out, "%s,0x%08X,%u,%u", r->name, r->start_address, r->size, r->used);
        if (config->show_percent) {
            double pct = (r->size > 0) ? (100.0 * r->used / r->size) : 0.0;
            fprintf(out, ",%.2f", pct);
        }
        fprintf(out, "\n");
    }
}

static void report_json(const FlashLayout *layout, const ReportConfig *config, FILE *out) {
    fprintf(out, "{\n  \"regions\": [\n");
    int first = 1;
    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (!config->show_unused && r->used == 0) continue;

        if (!first) fprintf(out, ",\n");
        first = 0;
        fprintf(out, "    {\"name\": \"%s\", \"start\": %u, \"size\": %u, \"used\": %u",
                r->name, r->start_address, r->size, r->used);
        if (config->show_percent) {
            double pct = (r->size > 0) ? (100.0 * r->used / r->size) : 0.0;
            fprintf(out, ", \"usage_pct\": %.2f", pct);
        }
        fprintf(out, "}");
    }
    fprintf(out, "\n  ]\n}\n");
}

int flashreport_generate(const FlashLayout *layout, const ReportConfig *config, FILE *out) {
    if (!layout || !config || !out) return -1;

    switch (config->format) {
        case REPORT_FORMAT_TEXT: report_text(layout, config, out); break;
        case REPORT_FORMAT_CSV:  report_csv(layout, config, out);  break;
        case REPORT_FORMAT_JSON: report_json(layout, config, out); break;
        default: return -1;
    }
    return 0;
}

void flashreport_summary(const FlashLayout *layout, FILE *out) {
    if (!layout || !out) return;

    unsigned int total_size = 0, total_used = 0;
    for (int i = 0; i < layout->region_count; i++) {
        total_size += layout->regions[i].size;
        total_used += layout->regions[i].used;
    }

    double pct = (total_size > 0) ? (100.0 * total_used / total_size) : 0.0;
    fprintf(out, "\nSummary: %u / %u bytes used (%.1f%%)\n", total_used, total_size, pct);
}
