#include "flashscale.h"
#include <stdlib.h>
#include <string.h>

FlashScaleResult flash_scale_region(const FlashRegion *region, double factor) {
    FlashScaleResult result = {0};
    if (!region || factor <= 0.0) {
        result.valid = false;
        return result;
    }
    result.original_size = region->size;
    result.scaled_size = (uint32_t)((double)region->size * factor);
    result.factor = factor;
    result.valid = true;
    return result;
}

FlashScaleMap flash_scale_layout(const FlashLayout *layout, double factor) {
    FlashScaleMap map = {0};
    if (!layout || factor <= 0.0 || layout->count == 0) {
        map.valid = false;
        return map;
    }
    map.count = layout->count;
    map.results = calloc(layout->count, sizeof(FlashScaleResult));
    if (!map.results) {
        map.valid = false;
        return map;
    }
    map.factor = factor;
    map.total_original = 0;
    map.total_scaled = 0;
    for (size_t i = 0; i < layout->count; i++) {
        map.results[i] = flash_scale_region(&layout->regions[i], factor);
        map.total_original += map.results[i].original_size;
        map.total_scaled  += map.results[i].scaled_size;
    }
    map.valid = true;
    return map;
}

bool flash_scale_fits(const FlashScaleMap *map, uint32_t flash_size) {
    if (!map || !map->valid) return false;
    return map->total_scaled <= flash_size;
}

void flash_scale_map_free(FlashScaleMap *map) {
    if (!map) return;
    free(map->results);
    map->results = NULL;
    map->count = 0;
    map->valid = false;
}
