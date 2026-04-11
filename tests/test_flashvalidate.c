#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashvalidate.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_layout(FlashRegion *regions, size_t count) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions = regions;
    layout.region_count = count;
    return layout;
}

static void test_null_layout(void) {
    FlashValidateResult r = flash_validate_layout(NULL);
    assert(!r.valid);
    assert(r.error_count == 1);
    assert(r.errors[0].code == FLASH_VALIDATE_EMPTY_LAYOUT);
    printf("PASS: test_null_layout\n");
}

static void test_empty_layout(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.region_count = 0;
    FlashValidateResult r = flash_validate_layout(&layout);
    assert(!r.valid);
    assert(r.error_count >= 1);
    assert(r.errors[0].code == FLASH_VALIDATE_EMPTY_LAYOUT);
    printf("PASS: test_empty_layout\n");
}

static void test_valid_layout(void) {
    FlashRegion regions[2];
    memset(regions, 0, sizeof(regions));
    strncpy(regions[0].name, "FLASH", FLASH_REGION_NAME_MAX - 1);
    regions[0].start = 0x08000000;
    regions[0].size  = 0x00080000;
    strncpy(regions[1].name, "RAM", FLASH_REGION_NAME_MAX - 1);
    regions[1].start = 0x20000000;
    regions[1].size  = 0x00020000;
    FlashLayout layout = make_layout(regions, 2);
    FlashValidateResult r = flash_validate_layout(&layout);
    assert(r.valid);
    assert(r.error_count == 0);
    printf("PASS: test_valid_layout\n");
}

static void test_zero_size_region(void) {
    FlashRegion regions[1];
    memset(regions, 0, sizeof(regions));
    strncpy(regions[0].name, "EMPTY", FLASH_REGION_NAME_MAX - 1);
    regions[0].start = 0x08000000;
    regions[0].size  = 0;
    FlashLayout layout = make_layout(regions, 1);
    FlashValidateResult r = flash_validate_layout(&layout);
    assert(!r.valid);
    assert(r.error_count >= 1);
    assert(r.errors[0].code == FLASH_VALIDATE_ZERO_SIZE);
    printf("PASS: test_zero_size_region\n");
}

static void test_overlapping_regions(void) {
    FlashRegion regions[2];
    memset(regions, 0, sizeof(regions));
    strncpy(regions[0].name, "A", FLASH_REGION_NAME_MAX - 1);
    regions[0].start = 0x08000000;
    regions[0].size  = 0x00010000;
    strncpy(regions[1].name, "B", FLASH_REGION_NAME_MAX - 1);
    regions[1].start = 0x08008000; /* overlaps A */
    regions[1].size  = 0x00010000;
    FlashLayout layout = make_layout(regions, 2);
    FlashValidateResult r = flash_validate_layout(&layout);
    assert(!r.valid);
    bool found_overlap = false;
    for (size_t i = 0; i < r.error_count; i++) {
        if (r.errors[i].code == FLASH_VALIDATE_OVERLAP) {
            found_overlap = true;
            break;
        }
    }
    assert(found_overlap);
    printf("PASS: test_overlapping_regions\n");
}

static void test_print_errors(void) {
    FlashRegion regions[1];
    memset(regions, 0, sizeof(regions));
    strncpy(regions[0].name, "X", FLASH_REGION_NAME_MAX - 1);
    regions[0].start = 0x00000000;
    regions[0].size  = 0;
    FlashLayout layout = make_layout(regions, 1);
    FlashValidateResult r = flash_validate_layout(&layout);
    flash_validate_print_errors(&r, stdout);
    printf("PASS: test_print_errors\n");
}

int main(void) {
    test_null_layout();
    test_empty_layout();
    test_valid_layout();
    test_zero_size_region();
    test_overlapping_regions();
    test_print_errors();
    printf("All flashvalidate tests passed.\n");
    return 0;
}
