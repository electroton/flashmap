/**
 * flashcluster.h - Flash region clustering utilities
 *
 * Groups adjacent or overlapping flash regions into logical clusters
 * based on proximity thresholds and region attributes.
 */

#ifndef FLASHCLUSTER_H
#define FLASHCLUSTER_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>
#include <stddef.h>

/**
 * A cluster is a collection of flash regions that are logically grouped
 * together based on adjacency or proximity.
 */
typedef struct {
    FlashRegion *regions;   /* Array of regions in this cluster */
    size_t       count;     /* Number of regions in this cluster */
    uint32_t     base;      /* Lowest start address in cluster */
    uint32_t     end;       /* Highest end address in cluster */
    uint32_t     total_size;/* Sum of all region sizes */
} FlashCluster;

/**
 * Result of a clustering operation.
 */
typedef struct {
    FlashCluster *clusters; /* Array of clusters */
    size_t        count;    /* Number of clusters */
} FlashClusterResult;

/**
 * Group regions in a layout into clusters where the gap between
 * consecutive regions (sorted by address) is <= gap_threshold bytes.
 *
 * @param layout        Source flash layout
 * @param gap_threshold Max gap in bytes between regions to still cluster them
 * @param out           Output cluster result (caller must free with flash_cluster_result_free)
 * @return              0 on success, -1 on error
 */
int flash_cluster_regions(const FlashLayout *layout,
                          uint32_t gap_threshold,
                          FlashClusterResult *out);

/**
 * Print a human-readable summary of clusters to stdout.
 *
 * @param result  Cluster result to print
 */
void flash_cluster_print(const FlashClusterResult *result);

/**
 * Free all memory associated with a FlashClusterResult.
 *
 * @param result  Cluster result to free
 */
void flash_cluster_result_free(FlashClusterResult *result);

#endif /* FLASHCLUSTER_H */
