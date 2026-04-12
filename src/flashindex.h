/**
 * flashindex.h - Flash region index for fast address-based lookup
 *
 * Provides an indexed structure for efficiently querying flash regions
 * by address, enabling O(log n) lookups instead of linear scans.
 */

#ifndef FLASHINDEX_H
#define FLASHINDEX_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of regions the index can hold */
#define FLASH_INDEX_MAX_REGIONS 256

/**
 * FlashIndex - sorted index of flash regions for fast lookup
 */
typedef struct {
    const FlashRegion *entries[FLASH_INDEX_MAX_REGIONS];
    size_t count;
    int sorted; /* non-zero if entries are sorted by base address */
} FlashIndex;

/**
 * Initialize an empty flash index.
 */
void flash_index_init(FlashIndex *index);

/**
 * Add a region to the index. Returns 0 on success, -1 if full.
 */
int flash_index_add(FlashIndex *index, const FlashRegion *region);

/**
 * Sort the index by base address. Must be called before lookup.
 */
void flash_index_sort(FlashIndex *index);

/**
 * Find the region containing the given address.
 * Returns a pointer to the region, or NULL if not found.
 * Index must be sorted before calling this.
 */
const FlashRegion *flash_index_find(const FlashIndex *index, uint32_t address);

/**
 * Find all regions overlapping [start, start+size).
 * Stores up to max_results pointers in out[]. Returns count found.
 */
size_t flash_index_find_range(const FlashIndex *index,
                              uint32_t start, uint32_t size,
                              const FlashRegion **out, size_t max_results);

/**
 * Clear all entries from the index.
 */
void flash_index_clear(FlashIndex *index);

#ifdef __cplusplus
}
#endif

#endif /* FLASHINDEX_H */
