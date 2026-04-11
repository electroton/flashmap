/**
 * flashoverlay.h - Flash region overlay detection and reporting
 *
 * Detects overlapping regions in a flash layout and reports conflicts.
 */

#ifndef FLASHOVERLAY_H
#define FLASHOVERLAY_H

#include "flashlayout.h"
#include "flashregion.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represents a detected overlap between two flash regions.
 */
typedef struct {
    const FlashRegion *region_a;
    const FlashRegion *region_b;
    uint32_t overlap_start;
    uint32_t overlap_end;
    uint32_t overlap_size;
} FlashOverlap;

/**
 * Result container for overlay detection.
 */
typedef struct {
    FlashOverlap *overlaps;
    int count;
    int capacity;
} FlashOverlayResult;

/**
 * Initialize an overlay result structure.
 * @param result Pointer to result to initialize.
 */
void flashoverlay_result_init(FlashOverlayResult *result);

/**
 * Free resources held by an overlay result.
 * @param result Pointer to result to free.
 */
void flashoverlay_result_free(FlashOverlayResult *result);

/**
 * Detect overlapping regions in the given flash layout.
 * @param layout  The flash layout to inspect.
 * @param result  Output parameter populated with detected overlaps.
 * @return Number of overlaps found, or -1 on error.
 */
int flashoverlay_detect(const FlashLayout *layout, FlashOverlayResult *result);

/**
 * Print a human-readable report of detected overlaps to stdout.
 * @param result Populated overlay result.
 */
void flashoverlay_print_report(const FlashOverlayResult *result);

#ifdef __cplusplus
}
#endif

#endif /* FLASHOVERLAY_H */
