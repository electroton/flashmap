#ifndef FLASHTHRESHOLD_REPORT_H
#define FLASHTHRESHOLD_REPORT_H

#include "flashthreshold.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void flash_threshold_report_print(const FlashThresholdResult *results,
                                  int count,
                                  FILE *out);

#ifdef __cplusplus
}
#endif

#endif /* FLASHTHRESHOLD_REPORT_H */
