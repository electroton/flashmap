/**
 * flashcrop.h - Flash region cropping and trimming utilities
 *
 * Provides functions to crop a flash layout to a specified address range,
 * trimming or excluding regions that fall outside the given bounds.
 */

#ifndef FLASHCROP_H
#define FLASHCROP_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"
#include "flashlayout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Crop mode controlling how boundary-straddling regions are handled.
 */
typedef enum {
    CROP_MODE_EXCLUDE = 0,  /**< Exclude regions that partially overlap the crop window */
    CROP_MODE_TRIM    = 1   /**< Trim regions that partially overlap to fit within the window */
} FlashCropMode;

/**
 * Result of a crop operation.
 */
typedef struct {
    FlashLayout *layout;    /**< Resulting cropped layout (caller must free) */
    size_t       excluded;  /**< Number of regions excluded or trimmed away */
    size_t       trimmed;   /**< Number of regions that were trimmed (CROP_MODE_TRIM only) */
} FlashCropResult;

/**
 * Crop the given layout to the address window [start, start+size).
 *
 * @param layout  Source layout to crop (not modified).
 * @param start   Start address of the crop window.
 * @param size    Size of the crop window in bytes.
 * @param mode    How to handle regions that straddle the boundary.
 * @param result  Output structure populated on success.
 * @return        0 on success, -1 on allocation failure.
 */
int flash_crop(const FlashLayout *layout,
               uint32_t start, uint32_t size,
               FlashCropMode mode,
               FlashCropResult *result);

/**
 * Free resources held by a FlashCropResult (frees the inner layout).
 *
 * @param result  Result to free.
 */
void flash_crop_result_free(FlashCropResult *result);

#ifdef __cplusplus
}
#endif

#endif /* FLASHCROP_H */
