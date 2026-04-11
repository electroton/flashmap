#ifndef FLASHALIGN_H
#define FLASHALIGN_H

#include <stdint.h>
#include <stdbool.h>
#include "flashregion.h"
#include "flashlayout.h"

/* Alignment issue severity levels */
typedef enum {
    ALIGN_WARNING,
    ALIGN_ERROR
} AlignSeverity;

/* Represents a single alignment issue found in a region */
typedef struct {
    char region_name[64];
    uint32_t address;
    uint32_t size;
    uint32_t required_alignment;
    uint32_t actual_alignment;
    AlignSeverity severity;
    char message[128];
} AlignIssue;

/* Result of an alignment check across a layout */
typedef struct {
    AlignIssue *issues;
    int issue_count;
    int error_count;
    int warning_count;
} AlignReport;

/**
 * Check alignment of all regions in a layout.
 * @param layout       The flash layout to check.
 * @param alignment    Required alignment boundary (e.g. 4, 8, 16, 256).
 * @return AlignReport with all detected issues. Caller must free with flashalign_free_report().
 */
AlignReport flashalign_check(const FlashLayout *layout, uint32_t alignment);

/**
 * Returns true if the given address is aligned to the boundary.
 */
bool flashalign_is_aligned(uint32_t address, uint32_t alignment);

/**
 * Print a human-readable alignment report to stdout.
 */
void flashalign_print_report(const AlignReport *report);

/**
 * Free resources held by an AlignReport.
 */
void flashalign_free_report(AlignReport *report);

#endif /* FLASHALIGN_H */
