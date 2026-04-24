#ifndef FLASHGROUP_REPORT_H
#define FLASHGROUP_REPORT_H

#include "flashgroup.h"
#include <stdio.h>

/**
 * Print a compact one-line summary for a group to the given stream.
 * Format: "[GROUP] <name>: <count> regions, <total> bytes used"
 */
void flashgroup_report_summary(const FlashGroup *group, FILE *out);

/**
 * Print a detailed per-region table for the group.
 * Columns: index, name, start address, size, % of group total.
 */
void flashgroup_report_detail(const FlashGroup *group, FILE *out);

#endif /* FLASHGROUP_REPORT_H */
