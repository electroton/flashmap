#ifndef FLASHSTATS_REPORT_H
#define FLASHSTATS_REPORT_H

#include "flashstats.h"
#include <stdio.h>

/**
 * Output format for the statistics report.
 */
typedef enum {
    STATS_FMT_TEXT = 0,  /* Human-readable plain text */
    STATS_FMT_CSV,       /* Comma-separated values     */
    STATS_FMT_JSON       /* JSON object                */
} StatsFormat;

/**
 * Write a statistics report for `layout` to `fp` in the requested format.
 * Returns 0 on success, -1 on error.
 */
int flashstats_report(const FlashLayout *layout, FILE *fp, StatsFormat fmt);

/**
 * Write only the utilization bar line to fp (text mode helper).
 * bar_width controls the width of the bar (clamped to [4, 80]).
 */
void flashstats_report_bar(const FlashStats *stats, FILE *fp, int bar_width);

#endif /* FLASHSTATS_REPORT_H */
