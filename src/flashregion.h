#ifndef FLASHREGION_H
#define FLASHREGION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_REGION_NAME 64
#define MAX_REGIONS 64

typedef struct {
    char     name[MAX_REGION_NAME];
    uint32_t origin;   /* start address */
    uint32_t length;   /* size in bytes */
    uint32_t used;     /* bytes actually used */
} FlashRegion;

typedef struct {
    FlashRegion regions[MAX_REGIONS];
    size_t      count;
} FlashMap;

/**
 * Initialize a FlashMap, zeroing all fields.
 */
void flashmap_init(FlashMap *map);

/**
 * Add a region to the map. Returns 0 on success, -1 if map is full
 * or name is NULL/empty.
 */
int flashmap_add_region(FlashMap *map, const char *name,
                        uint32_t origin, uint32_t length);

/**
 * Set the used bytes for a named region.
 * Returns 0 on success, -1 if region not found.
 */
int flashmap_set_used(FlashMap *map, const char *name, uint32_t used);

/**
 * Find a region by name. Returns pointer to region or NULL.
 */
FlashRegion *flashmap_find(FlashMap *map, const char *name);

/**
 * Return free bytes in a region (length - used).
 * Returns 0 if region not found.
 */
uint32_t flashmap_free(const FlashRegion *region);

#endif /* FLASHREGION_H */
