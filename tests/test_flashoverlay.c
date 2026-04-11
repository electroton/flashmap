#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashoverlay.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static void test_no_overlaps(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashRegion r1 = { .name = "TEXT",  .start = 0x08000000, .size = 0x10000 };
    FlashRegion r2 = { .name = "DATA",  .start = 0x08010000, .size = 0x8000  };
    FlashRegion r3 = { .name = "BSS",   .start = 0x08018000, .size = 0x4000  };

    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);
    flashlayout_add_region(&layout, &r3);

    FlashOverlayResult result;
    flashoverlay_result_init(&result);

    int count = flashoverlay_detect(&layout, &result);
    assert(count == 0);
    assert(result.count == 0);

    flashoverlay_result_free(&result);
    flashlayout_free(&layout);
    printf("PASS: test_no_overlaps\n");
}

static void test_single_overlap(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashRegion r1 = { .name = "TEXT",  .start = 0x08000000, .size = 0x10000 };
    FlashRegion r2 = { .name = "DATA",  .start = 0x0800E000, .size = 0x8000  };

    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);

    FlashOverlayResult result;
    flashoverlay_result_init(&result);

    int count = flashoverlay_detect(&layout, &result);
    assert(count == 1);
    assert(result.count == 1);
    assert(result.overlaps[0].overlap_start == 0x0800E000);
    assert(result.overlaps[0].overlap_end   == 0x08010000);
    assert(result.overlaps[0].overlap_size  == 0x2000);

    flashoverlay_result_free(&result);
    flashlayout_free(&layout);
    printf("PASS: test_single_overlap\n");
}

static void test_multiple_overlaps(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashRegion r1 = { .name = "A", .start = 0x1000, .size = 0x1000 };
    FlashRegion r2 = { .name = "B", .start = 0x1800, .size = 0x1000 };
    FlashRegion r3 = { .name = "C", .start = 0x2400, .size = 0x1000 };
    FlashRegion r4 = { .name = "D", .start = 0x2800, .size = 0x0800 };

    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);
    flashlayout_add_region(&layout, &r3);
    flashlayout_add_region(&layout, &r4);

    FlashOverlayResult result;
    flashoverlay_result_init(&result);

    int count = flashoverlay_detect(&layout, &result);
    assert(count == 2);

    flashoverlay_result_free(&result);
    flashlayout_free(&layout);
    printf("PASS: test_multiple_overlaps\n");
}

static void test_empty_layout(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashOverlayResult result;
    flashoverlay_result_init(&result);

    int count = flashoverlay_detect(&layout, &result);
    assert(count == 0);

    flashoverlay_result_free(&result);
    flashlayout_free(&layout);
    printf("PASS: test_empty_layout\n");
}

int main(void) {
    printf("=== flashoverlay tests ===\n");
    test_no_overlaps();
    test_single_overlap();
    test_multiple_overlaps();
    test_empty_layout();
    printf("All flashoverlay tests passed.\n");
    return 0;
}
