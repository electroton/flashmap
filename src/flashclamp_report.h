#ifndef FLASHCLAMP_REPORT_H
#define FLASHCLAMP_REPORT_H

#include <stdio.h>
#include "flashclamp.h"

/*
 * Print a summary of a clamp operation to `fp`.
 * Shows original region count, clamp window, and resulting clamped regions.
 */
void flashclamp_report_print(FILE *fp,
                             const FlashClampBounds *bounds,
                             const FlashRegion *original, size_t original_count,
                             const FlashRegion *clamped,  size_t clamped_count);

#endif /* FLASHCLAMP_REPORT_H */
