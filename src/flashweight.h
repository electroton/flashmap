#ifndef FLASHWEIGHT_H
#define FLASHWEIGHT_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stddef.h>

/* Weight represents the relative size contribution of a region
 * expressed as a fraction of the total flash layout size. */

typedef struct {
    char name[64];
    uint32_t address;
    size_t size;
    double weight;       /* 0.0 - 1.0 */
    double weight_pct;   /* 0.0 - 100.0 */
} FlashWeightEntry;

typedef struct {
    FlashWeightEntry *entries;
    size_t count;
    size_t total_size;
    double entropy;  /* Shannon entropy of the weight distribution */
} FlashWeightResult;

/* Compute weights for all regions in a layout */
FlashWeightResult *flash_weight_compute(const FlashLayout *layout);

/* Return the heaviest region entry, or NULL if result is empty */
const FlashWeightEntry *flash_weight_heaviest(const FlashWeightResult *result);

/* Return the lightest region entry, or NULL if result is empty */
const FlashWeightEntry *flash_weight_lightest(const FlashWeightResult *result);

/* Compute Shannon entropy of the weight distribution */
double flash_weight_entropy(const FlashWeightResult *result);

void flash_weight_result_free(FlashWeightResult *result);

#endif /* FLASHWEIGHT_H */
