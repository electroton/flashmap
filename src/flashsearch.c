#include "flashsearch.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 8

int flash_search_result_init(FlashSearchResult *result)
{
    if (!result) return -1;
    result->matches = malloc(INITIAL_CAPACITY * sizeof(FlashRegion *));
    if (!result->matches) return -1;
    result->count = 0;
    result->capacity = INITIAL_CAPACITY;
    return 0;
}

void flash_search_result_free(FlashSearchResult *result)
{
    if (!result) return;
    free(result->matches);
    result->matches = NULL;
    result->count = 0;
    result->capacity = 0;
}

static int result_append(FlashSearchResult *result, FlashRegion *region)
{
    if (result->count >= result->capacity) {
        size_t new_cap = result->capacity * 2;
        FlashRegion **tmp = realloc(result->matches,
                                    new_cap * sizeof(FlashRegion *));
        if (!tmp) return -1;
        result->matches = tmp;
        result->capacity = new_cap;
    }
    result->matches[result->count++] = region;
    return 0;
}

static int region_matches(const FlashRegion *region, const FlashSearchQuery *query)
{
    if (query->flags & FLASH_SEARCH_BY_NAME) {
        if (!region->name || !query->name_substr) return 0;
        if (!strstr(region->name, query->name_substr)) return 0;
    }
    if (query->flags & FLASH_SEARCH_BY_ADDR) {
        if (region->start < query->addr_min) return 0;
        if (region->start > query->addr_max) return 0;
    }
    if (query->flags & FLASH_SEARCH_BY_SIZE) {
        if (region->size < query->size_min) return 0;
        if (region->size > query->size_max) return 0;
    }
    if (query->flags & FLASH_SEARCH_BY_TYPE) {
        if (region->type != query->region_type) return 0;
    }
    return 1;
}

int flash_search(const FlashLayout *layout, const FlashSearchQuery *query,
                 FlashSearchResult *result)
{
    if (!layout || !query || !result) return -1;

    for (size_t i = 0; i < layout->region_count; i++) {
        FlashRegion *r = &layout->regions[i];
        if (region_matches(r, query)) {
            if (result_append(result, r) != 0) return -1;
        }
    }
    return (int)result->count;
}

FlashRegion *flash_search_by_name(const FlashLayout *layout, const char *name)
{
    if (!layout || !name) return NULL;
    for (size_t i = 0; i < layout->region_count; i++) {
        if (layout->regions[i].name &&
            strcmp(layout->regions[i].name, name) == 0) {
            return &layout->regions[i];
        }
    }
    return NULL;
}

FlashRegion *flash_search_by_address(const FlashLayout *layout, uint32_t address)
{
    if (!layout) return NULL;
    for (size_t i = 0; i < layout->region_count; i++) {
        FlashRegion *r = &layout->regions[i];
        if (address >= r->start && address < r->start + r->size) {
            return r;
        }
    }
    return NULL;
}
