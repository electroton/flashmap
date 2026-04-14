#ifndef FLASHRANGE_REPORT_H
#define FLASHRANGE_REPORT_H

#include "flashrange.h"

/* Print a formatted report for an array of FlashRange entries */
void flashrange_report_print(const FlashRange *ranges, int count);

/* Print only ranges that overlap with the given reference range */
void flashrange_report_overlapping(const FlashRange *ranges, int count,
                                    const FlashRange *ref);

#endif /* FLASHRANGE_REPORT_H */
