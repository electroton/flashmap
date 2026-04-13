/*
 * flashtrace_report.h - Reporting utilities for flash trace
 */
#ifndef FLASHTRACE_REPORT_H
#define FLASHTRACE_REPORT_H

#include "flashtrace.h"

/* Print a formatted report of all trace events to stdout */
void flash_trace_report_full(const FlashTrace *trace);

/* Print per-region access counts */
void flash_trace_report_by_region(const FlashTrace *trace);

/* Export trace events to a CSV file */
int  flash_trace_report_csv(const FlashTrace *trace, const char *filepath);

#endif /* FLASHTRACE_REPORT_H */
