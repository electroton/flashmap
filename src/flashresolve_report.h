#ifndef FLASHRESOLVE_REPORT_H
#define FLASHRESOLVE_REPORT_H

#include <stdio.h>
#include "flashresolve.h"

/*
 * Print a single resolve result to the given stream.
 * Includes symbol/expression, resolved address, region name, and offset.
 */
void flash_resolve_report_result(FILE *out, const FlashResolveResult *result);

/*
 * Print a compact one-line summary of the resolve result.
 */
void flash_resolve_report_summary(FILE *out, const FlashResolveResult *result);

#endif /* FLASHRESOLVE_REPORT_H */
