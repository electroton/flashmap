#ifndef FLASHSORT_H
#define FLASHSORT_H

#include "flashlayout.h"

/* Sort criteria for flash regions */
typedef enum {
    FLASH_SORT_BY_ADDRESS,
    FLASH_SORT_BY_SIZE,
    FLASH_SORT_BY_NAME,
    FLASH_SORT_BY_USAGE
} FlashSortKey;

/* Sort order */
typedef enum {
    FLASH_SORT_ASCENDING,
    FLASH_SORT_DESCENDING
} FlashSortOrder;

/* Sort options */
typedef struct {
    FlashSortKey   key;
    FlashSortOrder order;
    int            stable; /* 1 = stable sort, 0 = unstable */
} FlashSortOptions;

/*
 * Sort the regions in a FlashLayout in-place according to options.
 * Returns 0 on success, -1 on error.
 */
int flash_sort(FlashLayout *layout, const FlashSortOptions *opts);

/*
 * Return a new FlashLayout with regions sorted according to options.
 * Caller is responsible for freeing the returned layout.
 * Returns NULL on error.
 */
FlashLayout *flash_sort_copy(const FlashLayout *layout, const FlashSortOptions *opts);

/*
 * Fill opts with default values (sort by address, ascending, stable).
 */
void flash_sort_options_default(FlashSortOptions *opts);

#endif /* FLASHSORT_H */
