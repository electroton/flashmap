#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashoverlay.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_layout_a(void) {
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    l.regions[0] = (FlashRegion){ .name = "boot",   .start = 0x08000000, .size = 0x4000 };
    l.regions[1] = (FlashRegion){ .name = "app",    .start = 0x08004000, .size = 0x8000 };
    l.region_count = 2;
    return l;
}

static FlashLayout make_layout_b(void) {
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    l.regions[0] = (FlashRegion){ .name = "shared", .start = 0x08003000, .size = 0x2000 };
    l.regions[1] = (FlashRegion){ .name = "data",   .start = 0x0800A000, .size = 0x2000 };
    l.region_count = 2;
    return l;
}

static void test_overlay_detect_overlaps(void) {
    FlashLayout a = make_layout_a();
    FlashLayout b = make_layout_b();
    FlashOverlap overlaps[16];

    FlashOverlayResult r = flash_overlay_detect(&a, &b, overlaps, 16);
    assert(r == 2);
    /* boot [0x08000000..0x08004000) vs shared [0x08003000..0x08005000) */
    assert(overlaps[0].overlap_start == 0x08003000);
    assert(overlaps[0].overlap_size  == 0x1000);
    /* app [0x08004000..0x0800C000) vs shared [0x08003000..0x08005000) */
    assert(overlaps[1].overlap_start == 0x08004000);
    assert(overlaps[1].overlap_size  == 0x1000);
    printf("PASS: test_overlay_detect_overlaps\n");
}

static void test_overlay_no_overlap(void) {
    FlashLayout a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    a.regions[0] = (FlashRegion){ .name = "r1", .start = 0x08000000, .size = 0x4000 };
    a.region_count = 1;
    b.regions[0] = (FlashRegion){ .name = "r2", .start = 0x08004000, .size = 0x4000 };
    b.region_count = 1;

    FlashOverlap overlaps[4];
    FlashOverlayResult r = flash_overlay_detect(&a, &b, overlaps, 4);
    assert(r == 0);
    printf("PASS: test_overlay_no_overlap\n");
}

static void test_overlay_has_conflict_true(void) {
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    l.regions[0] = (FlashRegion){ .name = "a", .start = 0x08000000, .size = 0x4000 };
    l.regions[1] = (FlashRegion){ .name = "b", .start = 0x08002000, .size = 0x4000 };
    l.region_count = 2;
    assert(flash_overlay_has_conflict(&l) == true);
    printf("PASS: test_overlay_has_conflict_true\n");
}

static void test_overlay_has_conflict_false(void) {
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    l.regions[0] = (FlashRegion){ .name = "a", .start = 0x08000000, .size = 0x4000 };
    l.regions[1] = (FlashRegion){ .name = "b", .start = 0x08004000, .size = 0x4000 };
    l.region_count = 2;
    assert(flash_overlay_has_conflict(&l) == false);
    printf("PASS: test_overlay_has_conflict_false\n");
}

static void test_overlay_null_inputs(void) {
    FlashOverlap overlaps[4];
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    assert(flash_overlay_detect(NULL, &l, overlaps, 4) == FLASH_OVERLAY_ERR_INVALID);
    assert(flash_overlay_detect(&l, NULL, overlaps, 4) == FLASH_OVERLAY_ERR_INVALID);
    assert(flash_overlay_has_conflict(NULL) == false);
    printf("PASS: test_overlay_null_inputs\n");
}

int main(void) {
    test_overlay_detect_overlaps();
    test_overlay_no_overlap();
    test_overlay_has_conflict_true();
    test_overlay_has_conflict_false();
    test_overlay_null_inputs();
    printf("All flashoverlay tests passed.\n");
    return 0;
}
