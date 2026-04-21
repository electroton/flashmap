/**
 * flashresize.h - Flash region resize operations
 *
 * Provides functionality to resize flash memory regions,
 * adjusting sizes and addresses while maintaining layout integrity.
 */

#ifndef FLASHRESIZE_H
#define FLASHRESIZE_H

#include "flashregion.h"
#include "flashlayout.h"

#include <stdint.h>
#include <stdbool.h>

/** Result codes for resize operations */
typedef enum {
    RESIZE_OK            = 0,
    RESIZE_ERR_OVERFLOW  = 1,  /**< Region would exceed flash bounds */
    RESIZE_ERR_OVERLAP   = 2,  /**< Region would overlap another */
    RESIZE_ERR_TOO_SMALL = 3,  /**< New size is smaller than used space */
    RESIZE_ERR_NOT_FOUND = 4,  /**< Named region not found in layout */
    RESIZE_ERR_ALIGN     = 5,  /**< New size violates alignment constraint */
    RESIZE_ERR_INVALID   = 6   /**< Invalid arguments */
} FlashResizeResult;

/** Options controlling resize behavior */
typedef struct {
    bool     allow_overlap_check; /**< Verify no overlap after resize */
    bool     align_to_boundary;   /**< Snap size to alignment boundary */
    uint32_t alignment;           /**< Alignment boundary in bytes (power of 2) */
    uint32_t flash_end;           /**< Upper bound of flash address space */
} FlashResizeOptions;

/**
 * Resize a region to the given new size.
 *
 * @param region   Region to resize (modified in place)
 * @param new_size New size in bytes
 * @param opts     Resize options (may be NULL for defaults)
 * @return         RESIZE_OK on success, error code otherwise
 */
FlashResizeResult flash_resize_region(FlashRegion *region,
                                      uint32_t new_size,
                                      const FlashResizeOptions *opts);

/**
 * Resize a named region within a layout.
 *
 * @param layout   Layout containing the region
 * @param name     Name of the region to resize
 * @param new_size New size in bytes
 * @param opts     Resize options (may be NULL for defaults)
 * @return         RESIZE_OK on success, error code otherwise
 */
FlashResizeResult flash_resize_named(FlashLayout *layout,
                                     const char *name,
                                     uint32_t new_size,
                                     const FlashResizeOptions *opts);

/**
 * Return a human-readable string for a resize result code.
 */
const char *flash_resize_result_str(FlashResizeResult result);

#endif /* FLASHRESIZE_H */
