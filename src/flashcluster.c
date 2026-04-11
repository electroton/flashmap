#include "flashcluster.h"
#include <stdlib.h>
#include <string.h>

static int region_compare_by_address(const void *a, const void *b) {
    const FlashRegion *ra = (const FlashRegion *)a;
    const FlashRegion *rb = (const FlashRegion *)b;
    if (ra->address < rb->address) return -1;
    if (ra->address > rb->address) return 1;
    return 0;
}

FlashClusterResult flash_cluster_by_proximity(const FlashRegion *regions,
                                               size_t count,
                                               uint32_t gap_threshold) {
    FlashClusterResult result = {0};
    if (!regions || count == 0) return result;

    FlashRegion *sorted = malloc(count * sizeof(FlashRegion));
    if (!sorted) return result;
    memcpy(sorted, regions, count * sizeof(FlashRegion));
    qsort(sorted, count, sizeof(FlashRegion), region_compare_by_address);

    result.clusters = malloc(count * sizeof(FlashCluster));
    if (!result.clusters) {
        free(sorted);
        return result;
    }

    size_t cluster_idx = 0;
    FlashCluster *cur = &result.clusters[0];
    cur->regions[0] = sorted[0];
    cur->count = 1;
    cur->start_address = sorted[0].address;
    cur->end_address = sorted[0].address + sorted[0].size;

    for (size_t i = 1; i < count; i++) {
        uint32_t prev_end = sorted[i - 1].address + sorted[i - 1].size;
        uint32_t gap = (sorted[i].address >= prev_end)
                       ? (sorted[i].address - prev_end)
                       : 0;

        if (gap <= gap_threshold && cur->count < FLASH_CLUSTER_MAX_REGIONS) {
            cur->regions[cur->count++] = sorted[i];
            uint32_t end = sorted[i].address + sorted[i].size;
            if (end > cur->end_address) cur->end_address = end;
        } else {
            cluster_idx++;
            cur = &result.clusters[cluster_idx];
            cur->regions[0] = sorted[i];
            cur->count = 1;
            cur->start_address = sorted[i].address;
            cur->end_address = sorted[i].address + sorted[i].size;
        }
    }

    result.count = cluster_idx + 1;
    free(sorted);
    return result;
}

void flash_cluster_result_free(FlashClusterResult *result) {
    if (result && result->clusters) {
        free(result->clusters);
        result->clusters = NULL;
        result->count = 0;
    }
}

uint32_t flash_cluster_total_size(const FlashCluster *cluster) {
    if (!cluster || cluster->count == 0) return 0;
    return cluster->end_address - cluster->start_address;
}

uint32_t flash_cluster_used_size(const FlashCluster *cluster) {
    if (!cluster) return 0;
    uint32_t used = 0;
    for (size_t i = 0; i < cluster->count; i++) {
        used += cluster->regions[i].size;
    }
    return used;
}
