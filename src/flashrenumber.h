#ifndef FLASHRENUMBER_H
#define FLASHRENUMBER_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FLASH_RENUMBER_OK          = 0,
    FLASH_RENUMBER_ERR_INVALID = 1,
    FLASH_RENUMBER_ERR_ALLOC   = 2
} FlashRenumberStatus;

typedef struct {
    uint32_t     start_id; /* First ID to assign               */
    uint32_t     step;     /* Increment between IDs (min 1)    */
    int          apply;    /* Non-zero: write IDs back to regions */
} FlashRenumberOptions;

typedef struct {
    FlashRegion *region;  /* Pointer to the affected region */
    uint32_t     old_id;
    uint32_t     new_id;
} FlashRenumberEntry;

typedef struct {
    FlashRenumberEntry *entries;
    size_t              count;
    FlashRenumberStatus status;
} FlashRenumberResult;

/**
 * Renumber regions sequentially in their current array order.
 */
FlashRenumberResult flash_renumber(FlashRegion *regions, size_t count,
                                    const FlashRenumberOptions *opts);

/**
 * Renumber regions sorted ascending by base_address.
 */
FlashRenumberResult flash_renumber_by_address(FlashRegion *regions, size_t count,
                                               const FlashRenumberOptions *opts);

/**
 * Free resources held by a FlashRenumberResult.
 */
void flash_renumber_result_free(FlashRenumberResult *result);

#ifdef __cplusplus
}
#endif

#endif /* FLASHRENUMBER_H */
