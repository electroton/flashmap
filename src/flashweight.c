#include "flashweight.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

FlashWeightResult *flash_weight_compute(const FlashLayout *layout) {
    if (!layout || layout->count == 0) return NULL;

    FlashWeightResult *result = calloc(1, sizeof(FlashWeightResult));
    if (!result) return NULL;

    result->entries = calloc(layout->count, sizeof(FlashWeightEntry));
    if (!result->entries) { free(result); return NULL; }

    result->count = layout->count;

    size_t total = 0;
    for (size_t i = 0; i < layout->count; i++) {
        total += layout->regions[i].size;
    }
    result->total_size = total;

    for (size_t i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        FlashWeightEntry *e = &result->entries[i];
        strncpy(e->name, r->name, sizeof(e->name) - 1);
        e->address = r->address;
        e->size = r->size;
        e->weight = (total > 0) ? (double)r->size / (double)total : 0.0;
        e->weight_pct = e->weight * 100.0;
    }

    result->entropy = flash_weight_entropy(result);
    return result;
}

const FlashWeightEntry *flash_weight_heaviest(const FlashWeightResult *result) {
    if (!result || result->count == 0) return NULL;
    const FlashWeightEntry *best = &result->entries[0];
    for (size_t i = 1; i < result->count; i++) {
        if (result->entries[i].weight > best->weight)
            best = &result->entries[i];
    }
    return best;
}

const FlashWeightEntry *flash_weight_lightest(const FlashWeightResult *result) {
    if (!result || result->count == 0) return NULL;
    const FlashWeightEntry *best = &result->entries[0];
    for (size_t i = 1; i < result->count; i++) {
        if (result->entries[i].weight < best->weight)
            best = &result->entries[i];
    }
    return best;
}

double flash_weight_entropy(const FlashWeightResult *result) {
    if (!result || result->count == 0) return 0.0;
    double entropy = 0.0;
    for (size_t i = 0; i < result->count; i++) {
        double w = result->entries[i].weight;
        if (w > 0.0)
            entropy -= w * log2(w);
    }
    return entropy;
}

void flash_weight_result_free(FlashWeightResult *result) {
    if (!result) return;
    free(result->entries);
    free(result);
}
