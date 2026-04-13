#ifndef FLASHENCODE_REPORT_H
#define FLASHENCODE_REPORT_H

#include "flashencode.h"
#include "flashlayout.h"
#include <stdio.h>

/**
 * flashencode_report — Print encoded representations of all regions
 * in a FlashLayout to a file stream.
 */

/**
 * Print a report of all regions encoded in the given format to fp.
 * Each line: <region_name>  <encoded_string>
 */
void flash_encode_report_layout(const FlashLayout *layout,
                                 FlashEncodeFormat fmt,
                                 FILE *fp);

#endif /* FLASHENCODE_REPORT_H */
