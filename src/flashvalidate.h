/**
 * flashvalidate.h - Flash memory layout validation
 *
 * Provides validation routines for flash regions and layouts,
 * checking for overlaps, out-of-bounds regions, and alignment issues.
 */

#ifndef FLASHVALIDATE_H
#define FLASHVALIDATE_H

#include "flashlayout.h"
#include "flashregion.h"

#include <stdint.h>
#include <stdbool.h>

/** Validation error codes */
typedef enum {
    VALIDATE_OK              = 0,
    VALIDATE_ERR_OVERLAP     = 1,
    VALIDATE_ERR_OUT_OF_BOUNDS = 2,
    VALIDATE_ERR_ZERO_SIZE   = 3,
    VALIDATE_ERR_MISALIGNED  = 4,
    VALIDATE_ERR_NULL        = 5
} ValidationError;

/** A single validation issue found during checking */
typedef struct {
    ValidationError code;
    char            message[256];
    const char     *region_a;  /* name of first involved region (may be NULL) */
    const char     *region_b;  /* name of second involved region (may be NULL) */
} ValidationIssue;

/** Result container for a full layout validation */
typedef struct {
    ValidationIssue *issues;   /* heap-allocated array of issues */
    int              count;    /* number of issues found */
} ValidationResult;

/**
 * Validate a single FlashRegion for basic sanity.
 * Returns VALIDATE_OK if valid, otherwise an error code.
 */
ValidationError flash_validate_region(const FlashRegion *region);

/**
 * Validate an entire FlashLayout: checks each region individually
 * and then checks for inter-region overlaps.
 *
 * Caller must free result.issues with flash_validate_result_free().
 */
ValidationResult flash_validate_layout(const FlashLayout *layout);

/** Free resources held by a ValidationResult. */
void flash_validate_result_free(ValidationResult *result);

/** Return a human-readable string for a ValidationError code. */
const char *flash_validate_error_str(ValidationError code);

#endif /* FLASHVALIDATE_H */
