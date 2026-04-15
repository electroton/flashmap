#ifndef FLASHWATERMARK_H
#define FLASHWATERMARK_H

#include "flashregion.h"
#include <stdint.h>
#include <stddef.h>

/*
 * flashwatermark — Track high-water mark usage per flash region.
 *
 * A watermark records the maximum observed used size for a region
 * over time, useful for detecting peak usage trends.
 */

typedef struct {
    char     name[64];     /* Region name */
    uint32_t base;         /* Region base address */
    uint32_t capacity;     /* Total region capacity in bytes */
    uint32_t peak_used;    /* Maximum observed used bytes */
    uint32_t current_used; /* Current used bytes */
    float    peak_pct;     /* Peak usage as percentage of capacity */
} FlashWatermark;

typedef struct {
    FlashWatermark *marks;
    size_t          count;
    size_t          capacity;
} FlashWatermarkSet;

/* Initialize a watermark set */
void flash_watermark_set_init(FlashWatermarkSet *set);

/* Free resources held by a watermark set */
void flash_watermark_set_free(FlashWatermarkSet *set);

/* Record a usage sample for a named region; updates peak if exceeded */
int flash_watermark_record(FlashWatermarkSet *set,
                           const char *name,
                           uint32_t base,
                           uint32_t capacity,
                           uint32_t used);

/* Find a watermark entry by region name; returns NULL if not found */
const FlashWatermark *flash_watermark_find(const FlashWatermarkSet *set,
                                           const char *name);

/* Reset peak for all entries to their current_used value */
void flash_watermark_reset_peaks(FlashWatermarkSet *set);

/* Print a human-readable watermark report to stdout */
void flash_watermark_print(const FlashWatermarkSet *set);

#endif /* FLASHWATERMARK_H */
