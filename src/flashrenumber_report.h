#ifndef FLASHRENUMBER_REPORT_H
#define FLASHRENUMBER_REPORT_H

#include <stdio.h>
#include "flashrenumber.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Print a human-readable renumbering report to `out`.
 * Lists each region with its old and new ID.
 */
void flash_renumber_report_print(const FlashRenumberResult *result, FILE *out);

/**
 * Print a compact one-line summary (total regions renumbered).
 */
void flash_renumber_report_summary(const FlashRenumberResult *result, FILE *out);

#ifdef __cplusplus
}
#endif

#endif /* FLASHRENUMBER_REPORT_H */
