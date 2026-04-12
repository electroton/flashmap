#ifndef FLASHPATCH_REPORT_H
#define FLASHPATCH_REPORT_H

#include "flashpatch.h"
#include "flashlayout.h"

/*
 * Print a human-readable diff between the original layout and the
 * patched layout produced by applying a patch set.
 */
void flashpatch_report(const FlashLayout *original,
                       const FlashLayout *patched,
                       const FlashPatchSet *ps);

#endif /* FLASHPATCH_REPORT_H */
