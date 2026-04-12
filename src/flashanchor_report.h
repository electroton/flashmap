#ifndef FLASHANCHOR_REPORT_H
#define FLASHANCHOR_REPORT_H

#include "flashanchor.h"
#include "flashregion.h"
#include <stddef.h>

/*
 * flashanchor_report: Formatted reporting for anchor validation results.
 */

/* Print a full validation report to stdout.
 * Returns number of violations found. */
int flash_anchor_report(const FlashAnchorSet *set,
                        const FlashRegion *regions, size_t region_count);

/* Write validation report to a file.
 * Returns number of violations found, or -1 on file error. */
int flash_anchor_report_to_file(const FlashAnchorSet *set,
                                const FlashRegion *regions,
                                size_t region_count,
                                const char *filepath);

#endif /* FLASHANCHOR_REPORT_H */
