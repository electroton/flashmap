#include "flashstats_report.h"
#include <stdio.h>
#include <string.h>

static int report_text(const FlashStats *s, FILE *fp)
{
    fprintf(fp, "=== Flash Statistics ===\n");
    fprintf(fp, "Regions        : %zu\n",   s->region_count);
    fprintf(fp, "Address range  : 0x%08X .. 0x%08X\n", s->min_address, s->max_address);
    fprintf(fp, "Address span   : %u bytes\n",  s->address_span);
    fprintf(fp, "Total size     : %u bytes\n",  s->total_size);
    fprintf(fp, "Used           : %u bytes\n",  s->used_size);
    fprintf(fp, "Free           : %u bytes\n",  s->free_size);
    fprintf(fp, "Utilization    : %.1f%%\n",    s->utilization_pct);
    fprintf(fp, "Largest region : %u bytes\n",  s->largest_region);
    fprintf(fp, "Smallest region: %u bytes\n",  s->smallest_region);
    fprintf(fp, "Avg region size: %.1f bytes\n", s->avg_region_size);

    char bar[84];
    int bw = 40;
    flashstats_utilization_bar(s, bar, bw);
    fprintf(fp, "Utilization    : %s %.1f%%\n", bar, s->utilization_pct);
    return 0;
}

static int report_csv(const FlashStats *s, FILE *fp)
{
    fprintf(fp, "region_count,total_size,used_size,free_size,"
                "utilization_pct,largest_region,smallest_region,"
                "avg_region_size,min_address,max_address,address_span\n");
    fprintf(fp, "%zu,%u,%u,%u,%.2f,%u,%u,%.2f,%u,%u,%u\n",
            s->region_count, s->total_size, s->used_size, s->free_size,
            s->utilization_pct, s->largest_region, s->smallest_region,
            s->avg_region_size, s->min_address, s->max_address, s->address_span);
    return 0;
}

static int report_json(const FlashStats *s, FILE *fp)
{
    fprintf(fp, "{\n");
    fprintf(fp, "  \"region_count\": %zu,\n",       s->region_count);
    fprintf(fp, "  \"total_size\": %u,\n",           s->total_size);
    fprintf(fp, "  \"used_size\": %u,\n",            s->used_size);
    fprintf(fp, "  \"free_size\": %u,\n",            s->free_size);
    fprintf(fp, "  \"utilization_pct\": %.2f,\n",   s->utilization_pct);
    fprintf(fp, "  \"largest_region\": %u,\n",      s->largest_region);
    fprintf(fp, "  \"smallest_region\": %u,\n",     s->smallest_region);
    fprintf(fp, "  \"avg_region_size\": %.2f,\n",   s->avg_region_size);
    fprintf(fp, "  \"min_address\": %u,\n",          s->min_address);
    fprintf(fp, "  \"max_address\": %u,\n",          s->max_address);
    fprintf(fp, "  \"address_span\": %u\n",          s->address_span);
    fprintf(fp, "}\n");
    return 0;
}

int flashstats_report(const FlashLayout *layout, FILE *fp, StatsFormat fmt)
{
    if (!layout || !fp) return -1;

    FlashStats stats;
    if (flashstats_compute(layout, &stats) != 0) return -1;

    switch (fmt) {
        case STATS_FMT_CSV:  return report_csv(&stats, fp);
        case STATS_FMT_JSON: return report_json(&stats, fp);
        default:             return report_text(&stats, fp);
    }
}

void flashstats_report_bar(const FlashStats *stats, FILE *fp, int bar_width)
{
    if (!stats || !fp) return;
    if (bar_width < 4)  bar_width = 4;
    if (bar_width > 80) bar_width = 80;
    char buf[84];
    flashstats_utilization_bar(stats, buf, bar_width);
    fprintf(fp, "%s %.1f%%\n", buf, stats->utilization_pct);
}
