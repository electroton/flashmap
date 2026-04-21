/**
 * flashrebase.h - Rebase flash regions to a new base address
 *
 * Provides utilities to shift an entire FlashLayout to a new origin,
 * useful when targeting different memory-mapped regions or when
 * comparing layouts built for different hardware variants.
 */

#ifndef FLASHREBASE_H
#define FLASHREBASE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "flashregion.h"
#include "flashlayout.h"

#define FLASH_REBASE_ADDR_MAX UINT32_MAX

/** Result codes for rebase operations */
typedef enum {
    FLASH_REBASE_OK            = 0,
    FLASH_REBASE_ERR_NULL      = -1,
    FLASH_REBASE_ERR_OVERFLOW  = -2,
    FLASH_REBASE_ERR_UNDERFLOW = -3,
    FLASH_REBASE_ERR_ALLOC     = -4
} FlashRebaseResult;

/** Options controlling rebase behaviour */
typedef struct {
    bool check_overflow; /**< Abort if any address would exceed UINT32_MAX */
    bool copy_names;     /**< Deep-copy region name strings into output */
} FlashRebaseOptions;

/**
 * Rebase all regions in @p layout so the lowest start address becomes
 * @p new_base.  The rebased layout is written to @p out.
 *
 * @param layout   Source layout (read-only).
 * @param new_base Desired base address for the lowest region.
 * @param opts     Behavioural options.
 * @param out      Output layout; caller must free with flash_rebase_layout_free().
 * @return FLASH_REBASE_OK on success, error code otherwise.
 */
FlashRebaseResult flash_rebase(const FlashLayout *layout,
                                uint32_t new_base,
                                FlashRebaseOptions opts,
                                FlashLayout *out);

/** Return a human-readable string for a result code. */
const char *flash_rebase_result_str(FlashRebaseResult result);

/**
 * Free a layout produced by flash_rebase().
 * @param free_names If true, also free each region's name string.
 */
void flash_rebase_layout_free(FlashLayout *layout, bool free_names);

#endif /* FLASHREBASE_H */
