#ifndef FLASHREPORT_H
#define FLASHREPORT_H

#include "flashlayout.h"
#include <stdio.h>

/**
 * Output format for flash reports.
 */
typedef enum {
    REPORT_FORMAT_TEXT = 0,
    REPORT_FORMAT_CSV,
    REPORT_FORMAT_JSON
} ReportFormat;

/**
 * Report configuration options.
 */
typedef struct {
    ReportFormat format;
    int show_unused;    /* Include unused/gap regions */
    int show_percent;   /* Show usage as percentage */
} ReportConfig;

/**
 * Initialize a ReportConfig with default values.
 *
 * @param config  Pointer to the config to initialize.
 */
void report_config_init(ReportConfig *config);

/**
 * Generate a flash usage report to the given output stream.
 *
 * @param layout  The flash layout to report on.
 * @param config  Report configuration options.
 * @param out     Output stream (e.g., stdout or a file).
 * @return        0 on success, -1 on error.
 */
int flashreport_generate(const FlashLayout *layout, const ReportConfig *config, FILE *out);

/**
 * Print a summary line showing total used vs total available flash.
 *
 * @param layout  The flash layout to summarize.
 * @param out     Output stream.
 */
void flashreport_summary(const FlashLayout *layout, FILE *out);

#endif /* FLASHREPORT_H */
