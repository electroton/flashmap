#include "flashmirror.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MIRROR_INITIAL_CAPACITY 8

FlashMirrorMap *flash_mirror_map_create(void) {
    FlashMirrorMap *map = malloc(sizeof(FlashMirrorMap));
    if (!map) return NULL;
    map->pairs = malloc(sizeof(FlashMirrorPair) * MIRROR_INITIAL_CAPACITY);
    if (!map->pairs) { free(map); return NULL; }
    map->count = 0;
    map->capacity = MIRROR_INITIAL_CAPACITY;
    return map;
}

void flash_mirror_map_free(FlashMirrorMap *map) {
    if (!map) return;
    free(map->pairs);
    free(map);
}

int flash_mirror_add(FlashMirrorMap *map,
                     const FlashRegion *primary,
                     const FlashRegion *mirror) {
    if (!map || !primary || !mirror) return -1;
    if (primary->size != mirror->size) return -1;
    if (map->count >= map->capacity) {
        int new_cap = map->capacity * 2;
        FlashMirrorPair *tmp = realloc(map->pairs,
                                        sizeof(FlashMirrorPair) * new_cap);
        if (!tmp) return -1;
        map->pairs = tmp;
        map->capacity = new_cap;
    }
    FlashMirrorPair *p = &map->pairs[map->count++];
    strncpy(p->primary_name, primary->name, sizeof(p->primary_name) - 1);
    p->primary_name[sizeof(p->primary_name) - 1] = '\0';
    strncpy(p->mirror_name, mirror->name, sizeof(p->mirror_name) - 1);
    p->mirror_name[sizeof(p->mirror_name) - 1] = '\0';
    p->primary_base = primary->base_address;
    p->mirror_base  = mirror->base_address;
    p->size         = primary->size;
    p->active       = true;
    return 0;
}

bool flash_mirror_is_pair(const FlashRegion *a, const FlashRegion *b) {
    if (!a || !b) return false;
    return (a->size == b->size) &&
           (a->base_address != b->base_address);
}

int flash_mirror_detect(FlashMirrorMap *map, const FlashLayout *layout) {
    if (!map || !layout) return -1;
    int found = 0;
    for (int i = 0; i < layout->region_count; i++) {
        for (int j = i + 1; j < layout->region_count; j++) {
            const FlashRegion *a = &layout->regions[i];
            const FlashRegion *b = &layout->regions[j];
            if (flash_mirror_is_pair(a, b)) {
                if (flash_mirror_add(map, a, b) == 0)
                    found++;
            }
        }
    }
    return found;
}

const FlashMirrorPair *flash_mirror_find(const FlashMirrorMap *map,
                                          const char *region_name) {
    if (!map || !region_name) return NULL;
    for (int i = 0; i < map->count; i++) {
        if (strcmp(map->pairs[i].primary_name, region_name) == 0 ||
            strcmp(map->pairs[i].mirror_name,  region_name) == 0) {
            return &map->pairs[i];
        }
    }
    return NULL;
}

void flash_mirror_print(const FlashMirrorMap *map) {
    if (!map) return;
    printf("Flash Mirror Map (%d pair(s)):\n", map->count);
    for (int i = 0; i < map->count; i++) {
        const FlashMirrorPair *p = &map->pairs[i];
        printf("  [%d] %s @ 0x%08X  <-->  %s @ 0x%08X  (size: 0x%X)%s\n",
               i, p->primary_name, p->primary_base,
               p->mirror_name, p->mirror_base, p->size,
               p->active ? "" : " [inactive]");
    }
}
