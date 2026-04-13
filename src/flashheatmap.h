/**
 * flashheatmap.h - Flash memory usage heatmap generation
 *
 * Produces a text-based heatmap showing region density/utilization
 * across the flash address space.
 */

#ifndef FLASHHEATMAP_H
#define FLASHHEATMAP_H

#include "flashlayout.h"
#include <stdint.h>
#include <stddef.h>

#define HEATMAP_MAX_COLS   64
#define HEATMAP_MAX_ROWS   32

/* Density thresholds (percentage of bucket used) */
#define HEATMAP_EMPTY      0
#define HEATMAP_SPARSE     25
#define HEATMAP_MODERATE   50
#define HEATMAP_DENSE      75
#define HEATMAP_FULL       100

typedef struct {
    uint32_t base_address;
    uint32_t total_size;
    size_t   cols;           /* number of horizontal buckets */
    size_t   rows;           /* number of vertical rows (for tall maps) */
    uint8_t  density[HEATMAP_MAX_ROWS][HEATMAP_MAX_COLS]; /* 0-100 percent */
} FlashHeatmap;

/**
 * Build a heatmap from a flash layout.
 * cols: number of columns (1..HEATMAP_MAX_COLS)
 * Returns 0 on success, -1 on error.
 */
int flashheatmap_build(FlashHeatmap *hm, const FlashLayout *layout, size_t cols);

/**
 * Render the heatmap to stdout using ASCII characters.
 * Characters: ' ' empty, '.' sparse, '+' moderate, '#' dense, '@' full
 */
void flashheatmap_print(const FlashHeatmap *hm);

/**
 * Return the ASCII character representing a given density (0-100).
 */
char flashheatmap_char(uint8_t density);

/**
 * Write heatmap legend to stdout.
 */
void flashheatmap_print_legend(void);

#endif /* FLASHHEATMAP_H */
