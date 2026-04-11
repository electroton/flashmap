#ifndef FLASHVALIDATE_H
#define FLASHVALIDATE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "flashlayout.h"
#include "flashregion.h"

#define FLASH_VALIDATE_MAX_ERRORS 32
#define FLASH_VALIDATE_MSG_MAX    128

/* Validation error codes */
typedef enum {
    FLASH_VALIDATE_OK           = 0,
    FLASH_VALIDATE_EMPTY_LAYOUT = 1,  /* Layout has no regions */
    FLASH_VALIDATE_ZERO_SIZE    = 2,  /* A region has size == 0 */
    FLASH_VALIDATE_OVERFLOW     = 3,  /* start + size wraps address space */
    FLASH_VALIDATE_OVERLAP      = 4   /* Two regions overlap */
} FlashValidateCode;

typedef struct {
    FlashValidateCode code;
    char region_name[FLASH_REGION_NAME_MAX];
    char message[FLASH_VALIDATE_MSG_MAX];
} FlashValidateError;

typedef struct {
    bool               valid;
    size_t             error_count;
    FlashValidateError errors[FLASH_VALIDATE_MAX_ERRORS];
} FlashValidateResult;

/**
 * Validate a flash layout for common structural errors.
 * Returns a result struct describing all detected issues.
 */
FlashValidateResult flash_validate_layout(const FlashLayout *layout);

/**
 * Print all validation errors to the given file stream.
 */
void flash_validate_print_errors(const FlashValidateResult *result, FILE *out);

#endif /* FLASHVALIDATE_H */
