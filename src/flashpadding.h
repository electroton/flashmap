#ifndef FLASHPADDING_H
#define FLASHPADDING_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/* Padding fill strategies */
typedef enum {
    FLASH_PAD_ZERO    = 0,
    FLASH_PAD_FF      = 1,
    FLASH_PAD_PATTERN = 2
} FlashPadFill;

typedef struct {
    uint32_t     start;       /* Start address of padding region */
    uint32_t     end;         /* End address (exclusive) */
    size_t       size;        /* Size in bytes */
    FlashPadFill fill;        /* Fill strategy */
    uint8_t      pattern;     /* Pattern byte (used when fill == FLASH_PAD_PATTERN) */
} FlashPaddingRegion;

typedef struct {
    FlashPaddingRegion *regions;
    size_t              count;
    size_t              capacity;
    size_t              total_padding; /* Sum of all padding sizes */
} FlashPaddingResult;

/* Compute padding gaps between flash regions to meet alignment */
FlashPaddingResult flash_padding_compute(const FlashRegion *regions,
                                         size_t count,
                                         uint32_t alignment,
                                         FlashPadFill fill,
                                         uint8_t pattern);

/* Free resources held by a FlashPaddingResult */
void flash_padding_result_free(FlashPaddingResult *result);

/* Return total padding bytes across all regions */
size_t flash_padding_total(const FlashPaddingResult *result);

/* Print a human-readable summary to stdout */
void flash_padding_print(const FlashPaddingResult *result);

#endif /* FLASHPADDING_H */
