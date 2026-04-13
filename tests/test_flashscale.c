#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../src/flashscale.h"
#include "../src/flashregion.h"
#include "../src/flashlayout.h"

static void test_scale_region_basic(void) {
    FlashRegion r = { .name = "CODE", .start = 0x08000000, .size = 1024 };
    FlashScaleResult res = flash_scale_region(&r, 2.0);
    assert(res.valid);
    assert(res.original_size == 1024);
    assert(res.scaled_size  == 2048);
    assert(fabs(res.factor - 2.0) < 1e-9);
    printf("PASS: test_scale_region_basic\n");
}

static void test_scale_region_half(void) {
    FlashRegion r = { .name = "DATA", .start = 0x20000000, .size = 512 };
    FlashScaleResult res = flash_scale_region(&r, 0.5);
    assert(res.valid);
    assert(res.scaled_size == 256);
    printf("PASS: test_scale_region_half\n");
}

static void test_scale_region_null(void) {
    FlashScaleResult res = flash_scale_region(NULL, 1.0);
    assert(!res.valid);
    printf("PASS: test_scale_region_null\n");
}

static void test_scale_region_zero_factor(void) {
    FlashRegion r = { .name = "BSS", .start = 0x20001000, .size = 256 };
    FlashScaleResult res = flash_scale_region(&r, 0.0);
    assert(!res.valid);
    printf("PASS: test_scale_region_zero_factor\n");
}

static void test_scale_layout_basic(void) {
    FlashRegion regions[2] = {
        { .name = "CODE", .start = 0x08000000, .size = 1024 },
        { .name = "DATA", .start = 0x20000000, .size = 512  }
    };
    FlashLayout layout = { .regions = regions, .count = 2 };
    FlashScaleMap map = flash_scale_layout(&layout, 2.0);
    assert(map.valid);
    assert(map.count == 2);
    assert(map.total_original == 1536);
    assert(map.total_scaled   == 3072);
    flash_scale_map_free(&map);
    printf("PASS: test_scale_layout_basic\n");
}

static void test_scale_layout_null(void) {
    FlashScaleMap map = flash_scale_layout(NULL, 1.0);
    assert(!map.valid);
    printf("PASS: test_scale_layout_null\n");
}

static void test_scale_fits_true(void) {
    FlashRegion regions[1] = {
        { .name = "CODE", .start = 0x08000000, .size = 1024 }
    };
    FlashLayout layout = { .regions = regions, .count = 1 };
    FlashScaleMap map = flash_scale_layout(&layout, 1.0);
    assert(flash_scale_fits(&map, 2048));
    flash_scale_map_free(&map);
    printf("PASS: test_scale_fits_true\n");
}

static void test_scale_fits_false(void) {
    FlashRegion regions[1] = {
        { .name = "CODE", .start = 0x08000000, .size = 1024 }
    };
    FlashLayout layout = { .regions = regions, .count = 1 };
    FlashScaleMap map = flash_scale_layout(&layout, 3.0);
    assert(!flash_scale_fits(&map, 2048));
    flash_scale_map_free(&map);
    printf("PASS: test_scale_fits_false\n");
}

int main(void) {
    test_scale_region_basic();
    test_scale_region_half();
    test_scale_region_null();
    test_scale_region_zero_factor();
    test_scale_layout_basic();
    test_scale_layout_null();
    test_scale_fits_true();
    test_scale_fits_false();
    printf("All flashscale tests passed.\n");
    return 0;
}
