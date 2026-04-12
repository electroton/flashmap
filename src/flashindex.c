#include "flashindex.h"
#include <string.h>
#include <stdlib.h>

void flash_index_init(FlashIndex *index) {
    if (!index) return;
    memset(index, 0, sizeof(*index));
    index->sorted = 0;
}

int flash_index_add(FlashIndex *index, const FlashRegion *region) {
    if (!index || !region) return -1;
    if (index->count >= FLASH_INDEX_MAX_REGIONS) return -1;
    index->entries[index->count++] = region;
    index->sorted = 0;
    return 0;
}

static int region_cmp(const void *a, const void *b) {
    const FlashRegion *ra = *(const FlashRegion **)a;
    const FlashRegion *rb = *(const FlashRegion **)b;
    if (ra->base < rb->base) return -1;
    if (ra->base > rb->base) return  1;
    return 0;
}

void flash_index_sort(FlashIndex *index) {
    if (!index || index->count == 0) return;
    qsort(index->entries, index->count, sizeof(index->entries[0]), region_cmp);
    index->sorted = 1;
}

const FlashRegion *flash_index_find(const FlashIndex *index, uint32_t address) {
    if (!index || !index->sorted || index->count == 0) return NULL;

    /* Binary search for candidate region */
    size_t lo = 0, hi = index->count;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        const FlashRegion *r = index->entries[mid];
        if (address < r->base) {
            hi = mid;
        } else if (address >= r->base + r->size) {
            lo = mid + 1;
        } else {
            return r; /* address is within [base, base+size) */
        }
    }
    return NULL;
}

size_t flash_index_find_range(const FlashIndex *index,
                              uint32_t start, uint32_t size,
                              const FlashRegion **out, size_t max_results) {
    if (!index || !out || max_results == 0) return 0;

    uint32_t end = start + size;
    size_t found = 0;

    for (size_t i = 0; i < index->count && found < max_results; i++) {
        const FlashRegion *r = index->entries[i];
        uint32_t r_end = r->base + r->size;
        /* Overlap condition: r.base < end && r_end > start */
        if (r->base < end && r_end > start) {
            out[found++] = r;
        }
    }
    return found;
}

void flash_index_clear(FlashIndex *index) {
    if (!index) return;
    index->count = 0;
    index->sorted = 0;
}
