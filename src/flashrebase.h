/**
 * flashrebase.h - Flash region address rebasing utilities
 *
 * Provides functionality to shift flash regions to a new base address,
 * useful when retargeting firmware for different memory-mapped hardware.
 */

#ifndef FLASHREBASE_H
#define FLASHREBASE_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Result codes for rebase operations.
 */
typedef enum {
    REBASE_OK            = 0,
    REBASE_ERR_NULL      = -1,
    REBASE_ERR_OVERFLOW  = -2,
    REBASE_ERR_UNDERFLOW = -3
} FlashRebaseResult;

/**
 * Rebase a single FlashRegion by applying an offset to its start address.
 *
 * @param region  Pointer to the region to rebase (modified in place).
 * @param offset  Signed offset in bytes to apply.
 * @return        REBASE_OK on success, or an error code.
 */
FlashRebaseResult flash_rebase_region(FlashRegion *region, int64_t offset);

/**
 * Rebase all regions in a FlashLayout by applying a uniform offset.
 *
 * @param layout  Pointer to the layout to rebase (modified in place).
 * @param offset  Signed offset in bytes to apply to every region.
 * @return        REBASE_OK on success, or the first error encountered.
 */
FlashRebaseResult flash_rebase_layout(FlashLayout *layout, int64_t offset);

/**
 * Rebase a layout so that its lowest-addressed region starts at new_base.
 *
 * @param layout    Pointer to the layout to rebase.
 * @param new_base  Desired start address for the lowest region.
 * @return          REBASE_OK on success, or an error code.
 */
FlashRebaseResult flash_rebase_layout_to(FlashLayout *layout, uint32_t new_base);

/**
 * Return the minimum start address across all regions in the layout.
 *
 * @param layout  Pointer to the layout.
 * @param out     Output parameter receiving the minimum address.
 * @return        REBASE_OK on success, REBASE_ERR_NULL if layout is empty.
 */
FlashRebaseResult flash_rebase_min_address(const FlashLayout *layout, uint32_t *out);

#ifdef __cplusplus
}
#endif

#endif /* FLASHREBASE_H */
