/**
 * flashtrend_report.h - Reporting utilities for flash trend analysis
 */

#ifndef FLASHTREND_REPORT_H
#define FLASHTREND_REPORT_H

#include "flashtrend.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Print a human-readable trend report to the given file stream.
 */
void flash_trend_report_print(FILE *out, const FlashTrendSeries *series,
                              const FlashTrendResult *result);

/**
 * Print a one-line summary suitable for tabular output.
 */
void flash_trend_report_summary(FILE *out, const FlashTrendSeries *series,
                                const FlashTrendResult *result);

#ifdef __cplusplus
}
#endif

#endif /* FLASHTREND_REPORT_H */
