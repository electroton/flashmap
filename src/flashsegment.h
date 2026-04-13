/**
 * flashsegment.h - Flash memory segment grouping and classification
 *
 * Provides functionality to group flash regions into named segments
 * (e.g., "code", "data", "bss") and query segment-level properties.
 */

#ifndef FLASHSEGMENT_H
#define FLASHSEGMENT_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#define FLASH_SEGMENT_NAME_MAX 64
#define FLASH_SEGMENT_MAX_REGIONS 32

typedef enum {
    SEGMENT_TYPE_CODE,
    SEGMENT_TYPE_DATA,
    SEGMENT_TYPE_BSS,
    SEGMENT_TYPE_RODATA,
    SEGMENT_TYPE_STACK,
    SEGMENT_TYPE_HEAP,
    SEGMENT_TYPE_CUSTOM,
    SEGMENT_TYPE_UNKNOWN
} FlashSegmentType;

typedef struct {
    char name[FLASH_SEGMENT_NAME_MAX];
    FlashSegmentType type;
    FlashRegion *regions[FLASH_SEGMENT_MAX_REGIONS];
    size_t region_count;
} FlashSegment;

typedef struct {
    FlashSegment *segments;
    size_t count;
    size_t capacity;
} FlashSegmentMap;

/* Lifecycle */
FlashSegmentMap *flash_segment_map_create(void);
void             flash_segment_map_free(FlashSegmentMap *map);

/* Building segments */
int  flash_segment_add(FlashSegmentMap *map, const char *name, FlashSegmentType type);
int  flash_segment_add_region(FlashSegmentMap *map, const char *segment_name, FlashRegion *region);

/* Queries */
FlashSegment       *flash_segment_find(FlashSegmentMap *map, const char *name);
uint32_t            flash_segment_total_size(const FlashSegment *seg);
uint32_t            flash_segment_start_address(const FlashSegment *seg);
uint32_t            flash_segment_end_address(const FlashSegment *seg);
FlashSegmentType    flash_segment_type_from_string(const char *type_str);
const char         *flash_segment_type_to_string(FlashSegmentType type);

#endif /* FLASHSEGMENT_H */
