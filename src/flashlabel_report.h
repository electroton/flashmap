#ifndef FLASHLABEL_REPORT_H
#define FLASHLABEL_REPORT_H

#include <stdio.h>
#include "flashlabel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Print a human-readable table of all labels in @set to @out.
 * Columns: name, address, size, kind, pinned, note.
 */
void flashlabel_report_print(const FlashLabelSet *set, FILE *out);

/**
 * Print only labels whose kind matches @kind.
 */
void flashlabel_report_by_kind(const FlashLabelSet *set,
                                FlashLabelKind kind, FILE *out);

#ifdef __cplusplus
}
#endif

#endif /* FLASHLABEL_REPORT_H */
