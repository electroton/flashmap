#ifndef FLASHMIRROR_REPORT_H
#define FLASHMIRROR_REPORT_H

#include "flashmirror.h"
#include <stdio.h>

/* Write a detailed mirror report to the given file stream */
void flash_mirror_report(const FlashMirrorMap *map, FILE *out);

#endif /* FLASHMIRROR_REPORT_H */
