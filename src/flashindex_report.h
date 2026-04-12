/**
 * flashindex_report.h - Reporting utilities for FlashIndex
 */

#ifndef FLASHINDEX_REPORT_H
#define FLASHINDEX_REPORT_H

#include <stdio.h>
#include "flashindex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Print a summary of the index contents to the given stream.
 */
void flash_index_report_print(const FlashIndex *index, FILE *out);

#ifdef __cplusplus
}
#endif

#endif /* FLASHINDEX_REPORT_H */
