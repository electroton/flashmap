#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flashcompress.h"

/* Heuristic: estimate RLE compressed size based on fill ratio */
static uint32_t estimate_rle(uint32_t used, float fill_ratio) {
    /* Highly filled regions compress less; sparse regions compress well */
    float sparsity = 1.0f - fill_ratio;
    float factor = 0.4f + (0.55f * fill_ratio) - (0.1f * sparsity);
    if (factor < 0.1f) factor = 0.1f;
    if (factor > 0.95f) factor = 0.95f;
    return (uint32_t)(used * factor);
}

/* Heuristic: estimate LZ compressed size */
static uint32_t estimate_lz(uint32_t used, float fill_ratio) {
    float factor = 0.3f + (0.45f * fill_ratio);
    if (factor < 0.1f) factor = 0.1f;
    if (factor > 0.90f) factor = 0.90f;
    return (uint32_t)(used * factor);
}

FlashCompressResult flash_compress_region(const FlashRegion *region,
                                           FlashCompressAlgo algo) {
    FlashCompressResult r;
    memset(&r, 0, sizeof(r));

    r.region_name = region->name;
    r.raw_size    = region->size;
    r.used_size   = region->used;
    r.algo        = algo;

    if (r.raw_size == 0) {
        r.fill_ratio              = 0.0f;
        r.estimated_compressed    = 0;
        r.compression_ratio       = 1.0f;
        return r;
    }

    r.fill_ratio = (float)r.used_size / (float)r.raw_size;

    switch (algo) {
        case COMPRESS_ALGO_RLE:
            r.estimated_compressed = estimate_rle(r.used_size, r.fill_ratio);
            break;
        case COMPRESS_ALGO_LZ:
            r.estimated_compressed = estimate_lz(r.used_size, r.fill_ratio);
            break;
        default:
            r.estimated_compressed = r.used_size;
            break;
    }

    if (r.estimated_compressed == 0) r.estimated_compressed = 1;
    r.compression_ratio = (float)r.used_size / (float)r.estimated_compressed;

    return r;
}

FlashCompressReport *flash_compress_analyze(const FlashLayout *layout,
                                             FlashCompressAlgo algo) {
    if (!layout || layout->count == 0) return NULL;

    FlashCompressReport *report = malloc(sizeof(FlashCompressReport));
    if (!report) return NULL;

    report->count   = layout->count;
    report->results = malloc(sizeof(FlashCompressResult) * report->count);
    if (!report->results) { free(report); return NULL; }

    report->total_raw                  = 0;
    report->total_estimated_compressed = 0;

    for (size_t i = 0; i < layout->count; i++) {
        report->results[i] = flash_compress_region(&layout->regions[i], algo);
        report->total_raw                  += report->results[i].raw_size;
        report->total_estimated_compressed += report->results[i].estimated_compressed;
    }

    report->overall_ratio = (report->total_estimated_compressed > 0)
        ? (float)report->total_raw / (float)report->total_estimated_compressed
        : 1.0f;

    return report;
}

void flash_compress_report_free(FlashCompressReport *report) {
    if (!report) return;
    free(report->results);
    free(report);
}

void flash_compress_report_print(const FlashCompressReport *report) {
    if (!report) return;
    const char *algo_names[] = { "RLE", "LZ", "None" };
    printf("Flash Compression Estimate\n");
    printf("%-20s %10s %10s %10s %8s %8s\n",
           "Region", "RawSize", "UsedSize", "Compressed", "FillPct", "Ratio");
    printf("%-20s %10s %10s %10s %8s %8s\n",
           "------", "-------", "--------", "----------", "-------", "-----");
    for (size_t i = 0; i < report->count; i++) {
        const FlashCompressResult *r = &report->results[i];
        printf("%-20s %10u %10u %10u %7.1f%% %7.2fx\n",
               r->region_name ? r->region_name : "?",
               r->raw_size, r->used_size, r->estimated_compressed,
               r->fill_ratio * 100.0f, r->compression_ratio);
    }
    printf("\nAlgorithm : %s\n", algo_names[report->results[0].algo]);
    printf("Total raw : %u bytes\n", report->total_raw);
    printf("Total est.: %u bytes\n", report->total_estimated_compressed);
    printf("Overall   : %.2fx\n",    report->overall_ratio);
}
