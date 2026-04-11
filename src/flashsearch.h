/**
 * flashsearch.h - Search and query functionality for flash memory regions
 */

#ifndef FLASHSEARCH_H
#define FLASHSEARCH_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Search criteria flags (can be OR'd together)
 */
#define FLASH_SEARCH_BY_NAME    (1 << 0)
#define FLASH_SEARCH_BY_ADDR    (1 << 1)
#define FLASH_SEARCH_BY_SIZE    (1 << 2)
#define FLASH_SEARCH_BY_TYPE    (1 << 3)

/**
 * Search query structure
 */
typedef struct {
    uint32_t flags;          /* Which criteria to apply */
    const char *name_substr; /* Substring to match in region name */
    uint32_t addr_min;       /* Minimum start address (inclusive) */
    uint32_t addr_max;       /* Maximum start address (inclusive) */
    uint32_t size_min;       /* Minimum region size in bytes */
    uint32_t size_max;       /* Maximum region size in bytes */
    int region_type;         /* Region type to match (if FLASH_SEARCH_BY_TYPE) */
} FlashSearchQuery;

/**
 * Search results structure
 */
typedef struct {
    FlashRegion **matches;   /* Array of pointers to matching regions */
    size_t count;            /* Number of matches found */
    size_t capacity;         /* Internal capacity */
} FlashSearchResult;

/**
 * Initialize a search result container
 * @param result Pointer to result structure to initialize
 * @return 0 on success, -1 on allocation failure
 */
int flash_search_result_init(FlashSearchResult *result);

/**
 * Free resources held by a search result container
 * @param result Pointer to result structure to free
 */
void flash_search_result_free(FlashSearchResult *result);

/**
 * Search a flash layout using the given query
 * @param layout  Layout to search within
 * @param query   Query criteria
 * @param result  Output: populated with matching regions
 * @return Number of matches found, or -1 on error
 */
int flash_search(const FlashLayout *layout, const FlashSearchQuery *query,
                 FlashSearchResult *result);

/**
 * Find a region by exact name match
 * @param layout Layout to search
 * @param name   Exact name to look for
 * @return Pointer to matching region, or NULL if not found
 */
FlashRegion *flash_search_by_name(const FlashLayout *layout, const char *name);

/**
 * Find the region that contains a given address
 * @param layout  Layout to search
 * @param address Address to locate
 * @return Pointer to containing region, or NULL if not found
 */
FlashRegion *flash_search_by_address(const FlashLayout *layout, uint32_t address);

#endif /* FLASHSEARCH_H */
