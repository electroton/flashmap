#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashgap.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_layout(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));

    layout.regions[0] = (FlashRegion){ .name = "bootloader", .start = 0x08000000, .size = 0x4000 };
    layout.regions[1] = (FlashRegion){ .name = "app",        .start = 0x08008000, .size = 0x10000 };
    layout.regions[2] = (FlashRegion){ .name = "config",     .start = 0x0801E000, .size = 0x2000 };
    layout.region_count = 3;
    return layout;
}

static void test_gap_basic(void) {
    FlashLayout layout = make_layout();
    FlashGap gaps[16];

    FlashGapResult r = flash_gap_find(&layout, 0x08000000, 0x20000, gaps, 16);
    assert(r == 2);
    /* Gap between bootloader end (0x08004000) and app start (0x08008000) */
    assert(gaps[0].start == 0x08004000);
    assert(gaps[0].size  == 0x4000);
    /* Gap between app end (0x08018000) and config start (0x0801E000) */
    assert(gaps[1].start == 0x08018000);
    assert(gaps[1].size  == 0x6000);
    printf("PASS: test_gap_basic\n");
}

static void test_gap_total(void) {
    FlashLayout layout = make_layout();
    FlashGap gaps[16];
    FlashGapResult r = flash_gap_find(&layout, 0x08000000, 0x20000, gaps, 16);
    assert(r == 2);
    uint32_t total = flash_gap_total(gaps, (size_t)r);
    assert(total == 0x4000 + 0x6000);
    printf("PASS: test_gap_total\n");
}

static void test_gap_no_gaps(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions[0] = (FlashRegion){ .name = "full", .start = 0x08000000, .size = 0x20000 };
    layout.region_count = 1;

    FlashGap gaps[16];
    FlashGapResult r = flash_gap_find(&layout, 0x08000000, 0x20000, gaps, 16);
    assert(r == 0);
    printf("PASS: test_gap_no_gaps\n");
}

static void test_gap_has_large(void) {
    FlashLayout layout = make_layout();
    FlashGap gaps[16];
    FlashGapResult r = flash_gap_find(&layout, 0x08000000, 0x20000, gaps, 16);
    assert(r > 0);
    assert(flash_gap_has_large(gaps, (size_t)r, 0x5000) == true);
    assert(flash_gap_has_large(gaps, (size_t)r, 0x8000) == false);
    printf("PASS: test_gap_has_large\n");
}

static void test_gap_null_inputs(void) {
    FlashGap gaps[4];
    assert(flash_gap_find(NULL, 0x0, 0x1000, gaps, 4) == FLASH_GAP_ERR_NULL);
    printf("PASS: test_gap_null_inputs\n");
}

static void test_gap_overflow(void) {
    FlashLayout layout = make_layout();
    FlashGap gaps[1];
    FlashGapResult r = flash_gap_find(&layout, 0x08000000, 0x20000, gaps, 1);
    assert(r == FLASH_GAP_ERR_OVERFLOW);
    printf("PASS: test_gap_overflow\n");
}

int main(void) {
    test_gap_basic();
    test_gap_total();
    test_gap_no_gaps();
    test_gap_has_large();
    test_gap_null_inputs();
    test_gap_overflow();
    printf("All flashgap tests passed.\n");
    return 0;
}
