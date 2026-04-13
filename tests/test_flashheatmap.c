#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashheatmap.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* Helper: build a simple layout with given regions */
static FlashLayout make_layout(FlashRegion *regions, size_t count) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions = regions;
    layout.region_count = count;
    return layout;
}

static void test_build_basic(void) {
    FlashRegion regions[2] = {
        { .name = "text", .address = 0x08000000, .size = 0x4000 },
        { .name = "data", .address = 0x08004000, .size = 0x4000 },
    };
    FlashLayout layout = make_layout(regions, 2);
    FlashHeatmap hm;

    int ret = flashheatmap_build(&hm, &layout, 8);
    assert(ret == 0);
    assert(hm.base_address == 0x08000000);
    assert(hm.total_size   == 0x8000);
    assert(hm.cols == 8);

    /* All buckets should be fully used (100%) */
    for (size_t c = 0; c < hm.cols; c++) {
        assert(hm.density[0][c] == 100);
    }
    printf("PASS: test_build_basic\n");
}

static void test_build_sparse(void) {
    /* One small region in a large space */
    FlashRegion regions[1] = {
        { .name = "boot", .address = 0x00000000, .size = 0x100 },
    };
    /* Manually set total range by adding a phantom zero-size sentinel
       via two regions spanning 0x0 - 0x10000 */
    FlashRegion r2[2] = {
        { .name = "boot",    .address = 0x00000000, .size = 0x100  },
        { .name = "padding", .address = 0x0000FF00, .size = 0x100  },
    };
    FlashLayout layout = make_layout(r2, 2);
    FlashHeatmap hm;

    int ret = flashheatmap_build(&hm, &layout, 16);
    assert(ret == 0);
    /* First bucket should be dense, last bucket should be dense,
       middle buckets should be empty */
    assert(hm.density[0][0]  > 0);
    assert(hm.density[0][15] > 0);
    assert(hm.density[0][7]  == 0);
    printf("PASS: test_build_sparse\n");
}

static void test_heatmap_char(void) {
    assert(flashheatmap_char(0)   == ' ');
    assert(flashheatmap_char(10)  == '.');
    assert(flashheatmap_char(25)  == '+');
    assert(flashheatmap_char(50)  == '#');
    assert(flashheatmap_char(75)  == '@');
    assert(flashheatmap_char(100) == '@');
    printf("PASS: test_heatmap_char\n");
}

static void test_build_invalid(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    FlashHeatmap hm;

    assert(flashheatmap_build(NULL,    &layout, 8)  == -1);
    assert(flashheatmap_build(&hm,     NULL,    8)  == -1);
    assert(flashheatmap_build(&hm,     &layout, 0)  == -1);
    assert(flashheatmap_build(&hm,     &layout, HEATMAP_MAX_COLS + 1) == -1);
    printf("PASS: test_build_invalid\n");
}

static void test_print_smoke(void) {
    FlashRegion regions[1] = {
        { .name = "flash", .address = 0x08000000, .size = 0x20000 },
    };
    FlashLayout layout = make_layout(regions, 1);
    FlashHeatmap hm;
    assert(flashheatmap_build(&hm, &layout, 32) == 0);
    /* Just ensure it doesn't crash */
    flashheatmap_print(&hm);
    printf("PASS: test_print_smoke\n");
}

int main(void) {
    test_build_basic();
    test_build_sparse();
    test_heatmap_char();
    test_build_invalid();
    test_print_smoke();
    printf("All flashheatmap tests passed.\n");
    return 0;
}
