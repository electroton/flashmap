#include "flashgroup.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashGroup *flashgroup_create(const char *name) {
    FlashGroup *group = calloc(1, sizeof(FlashGroup));
    if (!group) return NULL;
    strncpy(group->name, name, FLASH_GROUP_NAME_MAX - 1);
    group->count = 0;
    group->capacity = 8;
    group->regions = calloc(group->capacity, sizeof(FlashRegion *));
    if (!group->regions) {
        free(group);
        return NULL;
    }
    return group;
}

void flashgroup_destroy(FlashGroup *group) {
    if (!group) return;
    free(group->regions);
    free(group);
}

int flashgroup_add(FlashGroup *group, const FlashRegion *region) {
    if (!group || !region) return -1;
    if (group->count >= group->capacity) {
        size_t new_cap = group->capacity * 2;
        FlashRegion **tmp = realloc(group->regions, new_cap * sizeof(FlashRegion *));
        if (!tmp) return -1;
        group->regions = tmp;
        group->capacity = new_cap;
    }
    group->regions[group->count++] = (FlashRegion *)region;
    return 0;
}

int flashgroup_remove(FlashGroup *group, const char *region_name) {
    if (!group || !region_name) return -1;
    for (size_t i = 0; i < group->count; i++) {
        if (strcmp(group->regions[i]->name, region_name) == 0) {
            memmove(&group->regions[i], &group->regions[i + 1],
                    (group->count - i - 1) * sizeof(FlashRegion *));
            group->count--;
            return 0;
        }
    }
    return -1;
}

FlashRegion *flashgroup_find(const FlashGroup *group, const char *region_name) {
    if (!group || !region_name) return NULL;
    for (size_t i = 0; i < group->count; i++) {
        if (strcmp(group->regions[i]->name, region_name) == 0)
            return group->regions[i];
    }
    return NULL;
}

uint32_t flashgroup_total_size(const FlashGroup *group) {
    if (!group) return 0;
    uint32_t total = 0;
    for (size_t i = 0; i < group->count; i++)
        total += group->regions[i]->size;
    return total;
}

void flashgroup_print(const FlashGroup *group, FILE *out) {
    if (!group || !out) return;
    fprintf(out, "Group: %s (%zu regions, total %u bytes)\n",
            group->name, group->count, flashgroup_total_size(group));
    for (size_t i = 0; i < group->count; i++) {
        fprintf(out, "  [%zu] %-24s 0x%08X  %u bytes\n",
                i, group->regions[i]->name,
                group->regions[i]->start,
                group->regions[i]->size);
    }
}
