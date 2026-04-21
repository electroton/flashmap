#ifndef FLASHMIRROR_H
#define FLASHMIRROR_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>
#include <stdbool.h>

/* Mirror relationship between two flash regions */
typedef struct {
    char primary_name[64];
    char mirror_name[64];
    uint32_t primary_base;
    uint32_t mirror_base;
    uint32_t size;
    bool active;
} FlashMirrorPair;

/* Collection of mirror pairs in a layout */
typedef struct {
    FlashMirrorPair *pairs;
    int count;
    int capacity;
} FlashMirrorMap;

/* Initialize a mirror map */
FlashMirrorMap *flash_mirror_map_create(void);

/* Free a mirror map */
void flash_mirror_map_free(FlashMirrorMap *map);

/* Add a mirror pair: primary region mirrored at mirror region */
int flash_mirror_add(FlashMirrorMap *map,
                     const FlashRegion *primary,
                     const FlashRegion *mirror);

/* Detect mirror pairs automatically from a layout (same size, different base) */
int flash_mirror_detect(FlashMirrorMap *map, const FlashLayout *layout);

/* Check if two regions are mirrors of each other */
bool flash_mirror_is_pair(const FlashRegion *a, const FlashRegion *b);

/* Resolve the primary region for a given region name */
const FlashMirrorPair *flash_mirror_find(const FlashMirrorMap *map,
                                          const char *region_name);

/* Print mirror map summary to stdout */
void flash_mirror_print(const FlashMirrorMap *map);

#endif /* FLASHMIRROR_H */
