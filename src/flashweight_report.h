#ifndef FLASHWEIGHT_REPORT_H
#define FLASHWEIGHT_REPORT_H

#include "flashweight.h"
#include <stdio.h>

void flash_weight_report_print(const FlashWeightResult *result, FILE *out);
void flash_weight_report_print_summary(const FlashWeightResult *result, FILE *out);

#endif /* FLASHWEIGHT_REPORT_H */
