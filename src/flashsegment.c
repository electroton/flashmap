#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "flashsegment.h"

#define INITIAL_CAPACITY 8

FlashSegmentMap *flash_segment_map_create(void) {
    FlashSegmentMap *map = calloc(1, sizeof(FlashSegmentMap));
    if (!map) return NULL;
    map->segments = calloc(INITIAL_CAPACITY, sizeof(FlashSegment));
    if (!map->segments) { free(map); return NULL; }
    map->capacity = INITIAL_CAPACITY;
    map->count = 0;
    return map;
}

void flash_segment_map_free(FlashSegmentMap *map) {
    if (!map) return;
    free(map->segments);
    free(map);
}

int flash_segment_add(FlashSegmentMap *map, const char *name, FlashSegmentType type) {
    if (!map || !name) return -1;
    if (flash_segment_find(map, name)) return -1; /* already exists */

    if (map->count >= map->capacity) {
        size_t new_cap = map->capacity * 2;
        FlashSegment *resized = realloc(map->segments, new_cap * sizeof(FlashSegment));
        if (!resized) return -1;
        map->segments = resized;
        map->capacity = new_cap;
    }

    FlashSegment *seg = &map->segments[map->count++];
    memset(seg, 0, sizeof(FlashSegment));
    strncpy(seg->name, name, FLASH_SEGMENT_NAME_MAX - 1);
    seg->type = type;
    return 0;
}

int flash_segment_add_region(FlashSegmentMap *map, const char *segment_name, FlashRegion *region) {
    if (!map || !segment_name || !region) return -1;
    FlashSegment *seg = flash_segment_find(map, segment_name);
    if (!seg) return -1;
    if (seg->region_count >= FLASH_SEGMENT_MAX_REGIONS) return -1;
    seg->regions[seg->region_count++] = region;
    return 0;
}

FlashSegment *flash_segment_find(FlashSegmentMap *map, const char *name) {
    if (!map || !name) return NULL;
    for (size_t i = 0; i < map->count; i++) {
        if (strncmp(map->segments[i].name, name, FLASH_SEGMENT_NAME_MAX) == 0)
            return &map->segments[i];
    }
    return NULL;
}

uint32_t flash_segment_total_size(const FlashSegment *seg) {
    if (!seg) return 0;
    uint32_t total = 0;
    for (size_t i = 0; i < seg->region_count; i++)
        total += seg->regions[i]->size;
    return total;
}

uint32_t flash_segment_start_address(const FlashSegment *seg) {
    if (!seg || seg->region_count == 0) return 0;
    uint32_t start = seg->regions[0]->start;
    for (size_t i = 1; i < seg->region_count; i++)
        if (seg->regions[i]->start < start)
            start = seg->regions[i]->start;
    return start;
}

uint32_t flash_segment_end_address(const FlashSegment *seg) {
    if (!seg || seg->region_count == 0) return 0;
    uint32_t end = 0;
    for (size_t i = 0; i < seg->region_count; i++) {
        uint32_t r_end = seg->regions[i]->start + seg->regions[i]->size;
        if (r_end > end) end = r_end;
    }
    return end;
}

FlashSegmentType flash_segment_type_from_string(const char *type_str) {
    if (!type_str) return SEGMENT_TYPE_UNKNOWN;
    if (strcmp(type_str, "code")   == 0) return SEGMENT_TYPE_CODE;
    if (strcmp(type_str, "data")   == 0) return SEGMENT_TYPE_DATA;
    if (strcmp(type_str, "bss")    == 0) return SEGMENT_TYPE_BSS;
    if (strcmp(type_str, "rodata") == 0) return SEGMENT_TYPE_RODATA;
    if (strcmp(type_str, "stack")  == 0) return SEGMENT_TYPE_STACK;
    if (strcmp(type_str, "heap")   == 0) return SEGMENT_TYPE_HEAP;
    if (strcmp(type_str, "custom") == 0) return SEGMENT_TYPE_CUSTOM;
    return SEGMENT_TYPE_UNKNOWN;
}

const char *flash_segment_type_to_string(FlashSegmentType type) {
    switch (type) {
        case SEGMENT_TYPE_CODE:   return "code";
        case SEGMENT_TYPE_DATA:   return "data";
        case SEGMENT_TYPE_BSS:    return "bss";
        case SEGMENT_TYPE_RODATA: return "rodata";
        case SEGMENT_TYPE_STACK:  return "stack";
        case SEGMENT_TYPE_HEAP:   return "heap";
        case SEGMENT_TYPE_CUSTOM: return "custom";
        default:                  return "unknown";
    }
}
