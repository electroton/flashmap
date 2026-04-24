/**
 * flashgroup.h - Group flash regions by tag, type, or custom key
 *
 * Provides functionality to collect and organize flash regions into
 * named groups for batch operations and reporting.
 */

#ifndef FLASHGROUP_H
#define FLASHGROUP_H

#include "flashregion.h"
#include <stddef.h>

#define FLASH_GROUP_NAME_MAX  64
#define FLASH_GROUP_MAX       32
#define FLASH_GROUP_MEMBERS_MAX 64

typedef enum {
    FLASH_GROUP_BY_TAG   = 0,
    FLASH_GROUP_BY_TYPE  = 1,
    FLASH_GROUP_BY_NAME  = 2
} FlashGroupMode;

typedef struct {
    char              name[FLASH_GROUP_NAME_MAX];
    const FlashRegion *members[FLASH_GROUP_MEMBERS_MAX];
    size_t            count;
    uint32_t          total_size;
    uint32_t          total_used;
} FlashGroup;

typedef struct {
    FlashGroup groups[FLASH_GROUP_MAX];
    size_t     count;
} FlashGroupSet;

/**
 * Initialize an empty group set.
 */
void flash_groupset_init(FlashGroupSet *gs);

/**
 * Group an array of regions by the given mode.
 * Returns 0 on success, -1 on failure.
 */
int flash_group_regions(FlashGroupSet *gs,
                        const FlashRegion *regions,
                        size_t region_count,
                        FlashGroupMode mode);

/**
 * Find a group by name. Returns NULL if not found.
 */
FlashGroup *flash_group_find(FlashGroupSet *gs, const char *name);

/**
 * Print a summary of all groups to stdout.
 */
void flash_group_print(const FlashGroupSet *gs);

#endif /* FLASHGROUP_H */
