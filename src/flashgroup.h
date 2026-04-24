#ifndef FLASHGROUP_H
#define FLASHGROUP_H

#include "flashregion.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define FLASH_GROUP_NAME_MAX 64

/**
 * FlashGroup — a named collection of FlashRegion pointers.
 * Regions are not owned by the group; callers manage region lifetimes.
 */
typedef struct {
    char         name[FLASH_GROUP_NAME_MAX];
    FlashRegion **regions;
    size_t        count;
    size_t        capacity;
} FlashGroup;

/** Create a new, empty group with the given name. */
FlashGroup *flashgroup_create(const char *name);

/** Free all memory owned by the group (not the regions themselves). */
void flashgroup_destroy(FlashGroup *group);

/** Add a region reference to the group. Returns 0 on success, -1 on error. */
int flashgroup_add(FlashGroup *group, const FlashRegion *region);

/** Remove a region by name. Returns 0 on success, -1 if not found. */
int flashgroup_remove(FlashGroup *group, const char *region_name);

/** Find and return a region by name, or NULL if not present. */
FlashRegion *flashgroup_find(const FlashGroup *group, const char *region_name);

/** Return the sum of sizes of all regions in the group. */
uint32_t flashgroup_total_size(const FlashGroup *group);

/** Print a human-readable summary of the group to the given stream. */
void flashgroup_print(const FlashGroup *group, FILE *out);

#endif /* FLASHGROUP_H */
