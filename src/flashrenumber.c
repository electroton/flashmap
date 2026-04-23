#include "flashrenumber.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashRenumberResult flash_renumber(FlashRegion *regions, size_t count,
                                    const FlashRenumberOptions *opts) {
    FlashRenumberResult result = {0};

    if (!regions || count == 0 || !opts) {
        result.status = FLASH_RENUMBER_ERR_INVALID;
        return result;
    }

    result.entries = malloc(sizeof(FlashRenumberEntry) * count);
    if (!result.entries) {
        result.status = FLASH_RENUMBER_ERR_ALLOC;
        return result;
    }

    uint32_t next_id = opts->start_id;
    uint32_t step    = opts->step > 0 ? opts->step : 1;

    for (size_t i = 0; i < count; i++) {
        result.entries[i].region    = &regions[i];
        result.entries[i].old_id    = regions[i].id;
        result.entries[i].new_id    = next_id;

        if (opts->apply) {
            regions[i].id = next_id;
        }

        next_id += step;
        result.count++;
    }

    result.status = FLASH_RENUMBER_OK;
    return result;
}

void flash_renumber_result_free(FlashRenumberResult *result) {
    if (!result) return;
    free(result->entries);
    result->entries = NULL;
    result->count   = 0;
}

FlashRenumberResult flash_renumber_by_address(FlashRegion *regions, size_t count,
                                               const FlashRenumberOptions *opts) {
    if (!regions || count == 0 || !opts) {
        FlashRenumberResult r = {0};
        r.status = FLASH_RENUMBER_ERR_INVALID;
        return r;
    }

    /* Sort indices by base address */
    size_t *order = malloc(sizeof(size_t) * count);
    if (!order) {
        FlashRenumberResult r = {0};
        r.status = FLASH_RENUMBER_ERR_ALLOC;
        return r;
    }

    for (size_t i = 0; i < count; i++) order[i] = i;

    /* Bubble sort by base address (small N expected) */
    for (size_t i = 0; i < count - 1; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (regions[order[i]].base_address > regions[order[j]].base_address) {
                size_t tmp = order[i];
                order[i]   = order[j];
                order[j]   = tmp;
            }
        }
    }

    FlashRenumberResult result = {0};
    result.entries = malloc(sizeof(FlashRenumberEntry) * count);
    if (!result.entries) {
        free(order);
        result.status = FLASH_RENUMBER_ERR_ALLOC;
        return result;
    }

    uint32_t next_id = opts->start_id;
    uint32_t step    = opts->step > 0 ? opts->step : 1;

    for (size_t i = 0; i < count; i++) {
        size_t idx = order[i];
        result.entries[i].region  = &regions[idx];
        result.entries[i].old_id  = regions[idx].id;
        result.entries[i].new_id  = next_id;

        if (opts->apply) {
            regions[idx].id = next_id;
        }

        next_id += step;
        result.count++;
    }

    free(order);
    result.status = FLASH_RENUMBER_OK;
    return result;
}
