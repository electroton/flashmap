#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/flashcompress.h"
#include "../src/flashregion.h"
#include "../src/flashlayout.h"

/* ---- helpers ---- */
static FlashRegion make_region(const char *name, uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    r.name = name;
    r.size = size;
    r.used = used;
    return r;
}

static FlashLayout make_layout(FlashRegion *regions, size_t count) {
    FlashLayout l;
    l.regions = regions;
    l.count   = count;
    return l;
}

/* ---- tests ---- */

static void test_compress_region_rle_basic(void) {
    FlashRegion r = make_region("FLASH", 65536, 32768);
    FlashCompressResult res = flash_compress_region(&r, COMPRESS_ALGO_RLE);

    assert(res.raw_size  == 65536);
    assert(res.used_size == 32768);
    assert(res.fill_ratio > 0.0f && res.fill_ratio <= 1.0f);
    assert(res.estimated_compressed > 0);
    assert(res.estimated_compressed <= res.used_size);
    assert(res.compression_ratio >= 1.0f);
    printf("PASS: test_compress_region_rle_basic\n");
}

static void test_compress_region_lz_better_than_rle(void) {
    FlashRegion r = make_region("CODE", 131072, 65536);
    FlashCompressResult rle = flash_compress_region(&r, COMPRESS_ALGO_RLE);
    FlashCompressResult lz  = flash_compress_region(&r, COMPRESS_ALGO_LZ);

    /* LZ should generally yield a smaller or equal estimate */
    assert(lz.estimated_compressed <= rle.estimated_compressed);
    printf("PASS: test_compress_region_lz_better_than_rle\n");
}

static void test_compress_region_zero_size(void) {
    FlashRegion r = make_region("EMPTY", 0, 0);
    FlashCompressResult res = flash_compress_region(&r, COMPRESS_ALGO_RLE);

    assert(res.fill_ratio       == 0.0f);
    assert(res.estimated_compressed == 0);
    assert(res.compression_ratio == 1.0f);
    printf("PASS: test_compress_region_zero_size\n");
}

static void test_compress_analyze_layout(void) {
    FlashRegion regions[3] = {
        make_region("FLASH",  65536, 40000),
        make_region("EEPROM", 4096,  1024),
        make_region("RAM",    8192,  8000)
    };
    FlashLayout layout = make_layout(regions, 3);

    FlashCompressReport *report = flash_compress_analyze(&layout, COMPRESS_ALGO_LZ);
    assert(report != NULL);
    assert(report->count == 3);
    assert(report->total_raw == 65536 + 4096 + 8192);
    assert(report->total_estimated_compressed > 0);
    assert(report->total_estimated_compressed <= report->total_raw);
    assert(report->overall_ratio >= 1.0f);

    flash_compress_report_free(report);
    printf("PASS: test_compress_analyze_layout\n");
}

static void test_compress_analyze_null_layout(void) {
    FlashCompressReport *report = flash_compress_analyze(NULL, COMPRESS_ALGO_RLE);
    assert(report == NULL);
    printf("PASS: test_compress_analyze_null_layout\n");
}

static void test_compress_report_print(void) {
    FlashRegion regions[2] = {
        make_region("BOOT", 16384, 8192),
        make_region("APP",  49152, 30000)
    };
    FlashLayout layout = make_layout(regions, 2);
    FlashCompressReport *report = flash_compress_analyze(&layout, COMPRESS_ALGO_RLE);
    assert(report != NULL);
    /* Should not crash */
    flash_compress_report_print(report);
    flash_compress_report_free(report);
    printf("PASS: test_compress_report_print\n");
}

int main(void) {
    test_compress_region_rle_basic();
    test_compress_region_lz_better_than_rle();
    test_compress_region_zero_size();
    test_compress_analyze_layout();
    test_compress_analyze_null_layout();
    test_compress_report_print();
    printf("All flashcompress tests passed.\n");
    return 0;
}
