/*
 * flashboundary.c - Flash region boundary checking and enforcement
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "flashboundary.h"

FlashBoundaryResult flash_boundary_check(const FlashRegion *region,
                                          uint32_t address,
                                          uint32_t size) {
    if (!region) return FLASH_BOUNDARY_INVALID_ARG;

    uint32_t region_end = region->address + region->size;
    uint32_t access_end = address + size;

    if (address < region->address) return FLASH_BOUNDARY_UNDERFLOW;
    if (access_end > region_end)   return FLASH_BOUNDARY_OVERFLOW;
    if (size == 0)                 return FLASH_BOUNDARY_ZERO_SIZE;

    return FLASH_BOUNDARY_OK;
}

bool flash_boundary_contains(const FlashRegion *region, uint32_t address) {
    if (!region) return false;
    return address >= region->address &&
           address < (region->address + region->size);
}

bool flash_boundary_overlaps(const FlashRegion *a, const FlashRegion *b) {
    if (!a || !b) return false;
    uint32_t a_end = a->address + a->size;
    uint32_t b_end = b->address + b->size;
    return a->address < b_end && b->address < a_end;
}

FlashBoundaryViolation *flash_boundary_find_violations(
        const FlashRegion *regions, size_t count, size_t *out_count) {
    if (!regions || count == 0 || !out_count) {
        if (out_count) *out_count = 0;
        return NULL;
    }

    /* Upper bound: n*(n-1)/2 pairs */
    size_t max_violations = count * (count - 1) / 2;
    FlashBoundaryViolation *violations = calloc(max_violations,
                                                sizeof(FlashBoundaryViolation));
    if (!violations) { *out_count = 0; return NULL; }

    size_t v = 0;
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (flash_boundary_overlaps(&regions[i], &regions[j])) {
                violations[v].region_a_index = i;
                violations[v].region_b_index = j;
                violations[v].overlap_start  =
                    regions[i].address > regions[j].address
                        ? regions[i].address : regions[j].address;
                uint32_t end_a = regions[i].address + regions[i].size;
                uint32_t end_b = regions[j].address + regions[j].size;
                violations[v].overlap_end = end_a < end_b ? end_a : end_b;
                v++;
            }
        }
    }

    *out_count = v;
    return violations;
}

void flash_boundary_free_violations(FlashBoundaryViolation *violations) {
    free(violations);
}

const char *flash_boundary_result_str(FlashBoundaryResult result) {
    switch (result) {
        case FLASH_BOUNDARY_OK:           return "OK";
        case FLASH_BOUNDARY_OVERFLOW:     return "overflow";
        case FLASH_BOUNDARY_UNDERFLOW:    return "underflow";
        case FLASH_BOUNDARY_ZERO_SIZE:    return "zero size";
        case FLASH_BOUNDARY_INVALID_ARG:  return "invalid argument";
        default:                          return "unknown";
    }
}
