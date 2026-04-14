#ifndef FLASHALIGN_REPORT_H
#define FLASHALIGN_REPORT_H

#include "flashalign.h"
#include <stdio.h>

/* Print a summary of alignment violations to the given stream */
void flashalign_report_violations(const FlashAlignResult *result, FILE *stream);

/* Print a detailed alignment report including per-region alignment status */
void flashalign_report_detailed(const FlashAlignResult *result,
                                 uint32_t alignment,
                                 FILE *stream);

/* Print a compact one-line alignment status suitable for CI output */
void flashalign_report_summary(const FlashAlignResult *result, FILE *stream);

#endif /* FLASHALIGN_REPORT_H */
