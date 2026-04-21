/**
 * flashresize_report.h - Reporting helpers for flash resize operations
 */

#ifndef FLASHRESIZE_REPORT_H
#define FLASHRESIZE_REPORT_H

#include "flashresize.h"
#include <stdio.h>

/**
 * Print a summary of a resize operation to the given stream.
 *
 * @param stream     Output stream (e.g. stdout)
 * @param name       Region name
 * @param old_size   Size before resize
 * @param new_size   Size after resize (or attempted)
 * @param result     Result code of the resize
 */
void flash_resize_report(FILE *stream,
                         const char *name,
                         uint32_t old_size,
                         uint32_t new_size,
                         FlashResizeResult result);

#endif /* FLASHRESIZE_REPORT_H */
