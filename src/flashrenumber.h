/**
 * flashrenumber.h - Renumber flash region addresses by applying a base offset
 * or remapping to a new sequential layout.
 */

#ifndef FLASHRENUMBER_H
#define FLASHRENUMBER_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Renumber mode controlling how addresses are reassigned.
 */
typedef enum {
    RENUMBER_SHIFT,      /* Shift all addresses by a fixed delta */
    RENUMBER_SEQUENTIAL  /* Pack regions sequentially from a base address */
} FlashRenumberMode;

/**
 * Options for the renumber operation.
 */
typedef struct {
    FlashRenumberMode mode;
    uint32_t          base;       /* Base address (SHIFT: added delta, SEQUENTIAL: start addr) */
    uint32_t          alignment;  /* Minimum alignment for sequential packing (0 = none) */
    bool              dry_run;    /* If true, compute result without modifying layout */
} FlashRenumberOptions;

/**
 * Result of a renumber operation.
 */
typedef struct {
    int      regions_renumbered;
    uint32_t min_old_addr;
    uint32_t max_old_addr;
    uint32_t min_new_addr;
    uint32_t max_new_addr;
    bool     had_overlap;  /* True if sequential packing detected overlap in original */
} FlashRenumberResult;

/**
 * Apply renumbering to all regions in a layout.
 *
 * @param layout   Layout to modify (or inspect if dry_run is set).
 * @param opts     Renumber options.
 * @param result   Output result summary (may be NULL).
 * @return 0 on success, -1 on error.
 */
int flash_renumber(FlashLayout *layout,
                   const FlashRenumberOptions *opts,
                   FlashRenumberResult *result);

/**
 * Print a human-readable summary of a renumber result.
 */
void flash_renumber_print_result(const FlashRenumberResult *result);

#endif /* FLASHRENUMBER_H */
