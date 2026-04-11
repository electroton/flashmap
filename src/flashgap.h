#ifndef FLASHGAP_H
#define FLASHGAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "flashlayout.h"
#include "flashregion.h"

/* Represents an unused gap between flash regions */
typedef struct {
    uint32_t start;       /* Start address of the gap */
    uint32_t size;        /* Size of the gap in bytes */
    const FlashRegion *before; /* Region immediately before the gap, or NULL */
    const FlashRegion *after;  /* Region immediately after the gap, or NULL */
} FlashGap;

typedef enum {
    FLASH_GAP_OK       = 0,
    FLASH_GAP_ERR_NULL = -1,
    FLASH_GAP_ERR_OVERFLOW = -2
} FlashGapResult;

/*
 * Find all unused gaps within [base_addr, base_addr+total_size).
 * Returns number of gaps found, or negative FlashGapResult on error.
 */
FlashGapResult flash_gap_find(const FlashLayout *layout,
                              uint32_t base_addr,
                              uint32_t total_size,
                              FlashGap *gaps,
                              size_t max_gaps);

/* Returns total unused bytes in the layout within the given range */
uint32_t flash_gap_total(const FlashGap *gaps, size_t count);

/* Returns true if any gap is larger than min_size */
bool flash_gap_has_large(const FlashGap *gaps, size_t count, uint32_t min_size);

/* Print gap report to the given file stream */
void flash_gap_print(const FlashGap *gaps, size_t count, FILE *out);

#endif /* FLASHGAP_H */
