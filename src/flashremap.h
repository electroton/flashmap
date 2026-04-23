/**
 * flashremap.h - Flash region address remapping support
 *
 * Provides functionality to remap flash regions from one address
 * space to another, useful for bootloader/application address
 * translation and memory-mapped I/O overlays.
 */

#ifndef FLASHREMAP_H
#define FLASHREMAP_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#define FLASHREMAP_MAX_ENTRIES 64
#define FLASHREMAP_NAME_MAX    32

typedef struct {
    char     name[FLASHREMAP_NAME_MAX];
    uint32_t src_base;   /* original base address */
    uint32_t dst_base;   /* remapped base address */
    uint32_t size;       /* region size in bytes */
} FlashRemapEntry;

typedef struct {
    FlashRemapEntry entries[FLASHREMAP_MAX_ENTRIES];
    size_t          count;
} FlashRemapTable;

/**
 * Initialize an empty remap table.
 */
void flashremap_init(FlashRemapTable *table);

/**
 * Add a remapping entry to the table.
 * Returns 0 on success, -1 if table is full or arguments are invalid.
 */
int flashremap_add(FlashRemapTable *table, const char *name,
                   uint32_t src_base, uint32_t dst_base, uint32_t size);

/**
 * Translate a source address to its remapped destination address.
 * Returns the translated address, or src_addr if no mapping matches.
 */
uint32_t flashremap_translate(const FlashRemapTable *table, uint32_t src_addr);

/**
 * Apply remapping table to a FlashRegion, updating its base address.
 * Returns 1 if the region was remapped, 0 if no mapping matched.
 */
int flashremap_apply(const FlashRemapTable *table, FlashRegion *region);

/**
 * Look up a remap entry by name.
 * Returns a pointer to the entry, or NULL if not found.
 */
const FlashRemapEntry *flashremap_find(const FlashRemapTable *table,
                                       const char *name);

/**
 * Remove all entries from the remap table.
 */
void flashremap_clear(FlashRemapTable *table);

#endif /* FLASHREMAP_H */
