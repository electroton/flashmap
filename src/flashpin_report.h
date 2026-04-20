#ifndef FLASHPIN_REPORT_H
#define FLASHPIN_REPORT_H

#include "flashpin.h"
#include <stdio.h>

/**
 * Print a formatted report of all pins in the set to the given stream.
 */
void flashpin_report_print(const FlashPinSet *ps, FILE *out);

/**
 * Print a single pin entry.
 */
void flashpin_report_print_pin(const FlashPin *pin, FILE *out);

#endif /* FLASHPIN_REPORT_H */
