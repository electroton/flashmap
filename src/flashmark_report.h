#ifndef FLASHMARK_REPORT_H
#define FLASHMARK_REPORT_H

#include "flashmark.h"
#include <stdio.h>

/*
 * Print a human-readable table of all marks in the set.
 * Output is written to the provided FILE stream.
 */
void flashmark_report_print(const FlashMarkSet *set, FILE *stream);

/*
 * Print only marks of a given type.
 */
void flashmark_report_by_type(const FlashMarkSet *set, FlashMarkType type,
                               FILE *stream);

#endif /* FLASHMARK_REPORT_H */
