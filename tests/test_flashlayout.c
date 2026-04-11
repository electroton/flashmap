#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashlayout.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';
    r.start_address = start;
    r.size = size;
    return r;
}

static void test_init(void) {
    FlashLayout layout;
    flashlayout_init(&layout);
    assert(layout.region_count == 0);
    assert(layout.total_flash_size == 0);
    printf("PASS: test_init\n");
}

static void test_add_region(void) {
    FlashLayout layout;
    flashlayout_init(&layout);
    layout.total_flash_size = 0x100000;

    FlashRegion r1 = make_region("bootloader", 0x08000000, 0x8000);
    FlashRegion r2 = make_region("application", 0x08008000, 0x70000);

    assert(flashlayout_add_region(&layout, &r1) == 0);
    assert(flashlayout_add_region(&layout, &r2) == 0);
    assert(layout.region_count == 2);
    printf("PASS: test_add_region\n");
}

static void test_overlap_rejected(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashRegion r1 = make_region("a", 0x08000000, 0x10000);
    FlashRegion r2 = make_region("b", 0x08008000, 0x10000); /* overlaps r1 */

    assert(flashlayout_add_region(&layout, &r1) == 0);
    assert(flashlayout_add_region(&layout, &r2) == -1);
    assert(layout.region_count == 1);
    printf("PASS: test_overlap_rejected\n");
}

static void test_used_free_bytes(void) {
    FlashLayout layout;
    flashlayout_init(&layout);
    layout.total_flash_size = 0x80000; /* 512 KB */

    FlashRegion r1 = make_region("boot", 0x08000000, 0x10000);
    FlashRegion r2 = make_region("app",  0x08010000, 0x20000);
    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);

    assert(flashlayout_used_bytes(&layout) == 0x30000);
    assert(flashlayout_free_bytes(&layout) == 0x50000);
    printf("PASS: test_used_free_bytes\n");
}

static void test_sort(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashRegion r1 = make_region("app",  0x08010000, 0x10000);
    FlashRegion r2 = make_region("boot", 0x08000000, 0x8000);
    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);

    flashlayout_sort(&layout);
    assert(layout.regions[0].start_address == 0x08000000);
    assert(layout.regions[1].start_address == 0x08010000);
    printf("PASS: test_sort\n");
}

int main(void) {
    test_init();
    test_add_region();
    test_overlap_rejected();
    test_used_free_bytes();
    test_sort();
    printf("All flashlayout tests passed.\n");
    return 0;
}
