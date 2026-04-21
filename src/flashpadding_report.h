#ifndef FLASHPADDING_REPORT_H
#define FLASHPADDING_REPORT_H

#include "flashpadding.h"
#include <stdio.h>

/* Write a CSV report of padding regions to the given file stream */
void flash_padding_report_csv(const FlashPaddingResult *result, FILE *out);

/* Write a plain-text summary report to the given file stream */
void flash_padding_report_text(const FlashPaddingResult *result, FILE *out);

#endif /* FLASHPADDING_REPORT_H */
