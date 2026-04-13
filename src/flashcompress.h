#ifndef FLASHCOMPRESS_H
#define FLASHCOMPRESS_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"
#include "flashlayout.h"

/*
 * flashcompress — estimates compressibility and reports potential
 * flash savings based on region fill patterns and entropy heuristics.
 */

typedef enum {
    COMPRESS_ALGO_RLE,
    COMPRESS_ALGO_LZ,
    COMPRESS_ALGO_NONE
} FlashCompressAlgo;

typedef struct {
    const char     *region_name;
    uint32_t        raw_size;        /* actual region size in bytes */
    uint32_t        used_size;       /* bytes actually used */
    uint32_t        estimated_compressed; /* estimated size after compression */
    float           compression_ratio;   /* raw / compressed */
    float           fill_ratio;          /* used / raw */
    FlashCompressAlgo algo;
} FlashCompressResult;

typedef struct {
    FlashCompressResult *results;
    size_t               count;
    uint32_t             total_raw;
    uint32_t             total_estimated_compressed;
    float                overall_ratio;
} FlashCompressReport;

/* Estimate compression for all regions in a layout */
FlashCompressReport *flash_compress_analyze(const FlashLayout *layout,
                                             FlashCompressAlgo algo);

/* Estimate compression for a single region */
FlashCompressResult flash_compress_region(const FlashRegion *region,
                                           FlashCompressAlgo algo);

/* Free report memory */
void flash_compress_report_free(FlashCompressReport *report);

/* Print report to stdout */
void flash_compress_report_print(const FlashCompressReport *report);

#endif /* FLASHCOMPRESS_H */
