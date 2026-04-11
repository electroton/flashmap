#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashsummary.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_test_layout(void) {
    FlashLayout layout;
    flashlayout_init(&layout);

    FlashRegion r1;
    strncpy(r1.name, "FLASH", FLASH_REGION_NAME_MAX);
    r1.start = 0x08000000;
    r1.size  = 512 * 1024;
    r1.used  = 300 * 1024;

    FlashRegion r2;
    strncpy(r2.name, "RAM", FLASH_REGION_NAME_MAX);
    r2.start = 0x20000000;
    r2.size  = 128 * 1024;
    r2.used  = 64 * 1024;

    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);
    return layout;
}

static void test_init(void) {
    FlashSummary s;
    flashsummary_init(&s);
    assert(s.total_regions == 0);
    assert(s.total_size == 0);
    assert(s.used_size == 0);
    assert(s.free_size == 0);
    assert(s.utilization_percent == 0.0f);
    printf("[PASS] test_init\n");
}

static void test_compute_totals(void) {
    FlashLayout layout = make_test_layout();
    FlashSummary s;
    flashsummary_init(&s);
    flashsummary_compute(&s, &layout);

    assert(s.total_regions == 2);
    assert(s.total_size == (512 + 128) * 1024);
    assert(s.used_size  == (300 + 64)  * 1024);
    assert(s.free_size  == (212 + 64)  * 1024);
    printf("[PASS] test_compute_totals\n");
}

static void test_compute_utilization(void) {
    FlashLayout layout = make_test_layout();
    FlashSummary s;
    flashsummary_init(&s);
    flashsummary_compute(&s, &layout);

    float expected = (float)(364 * 1024) / (float)(640 * 1024) * 100.0f;
    assert(s.utilization_percent > expected - 0.1f);
    assert(s.utilization_percent < expected + 0.1f);
    printf("[PASS] test_compute_utilization\n");
}

static void test_compute_region_bounds(void) {
    FlashLayout layout = make_test_layout();
    FlashSummary s;
    flashsummary_init(&s);
    flashsummary_compute(&s, &layout);

    assert(s.smallest_region == 128 * 1024);
    assert(s.largest_region  == 512 * 1024);
    printf("[PASS] test_compute_region_bounds\n");
}

static void test_compute_largest_free_block(void) {
    FlashLayout layout = make_test_layout();
    FlashSummary s;
    flashsummary_init(&s);
    flashsummary_compute(&s, &layout);

    /* FLASH free = 212K, RAM free = 64K => largest free = 212K */
    assert(s.largest_free_block == 212 * 1024);
    printf("[PASS] test_compute_largest_free_block\n");
}

static void test_null_safety(void) {
    flashsummary_init(NULL);
    flashsummary_compute(NULL, NULL);
    flashsummary_print(NULL);
    printf("[PASS] test_null_safety\n");
}

int main(void) {
    printf("Running flashsummary tests...\n");
    test_init();
    test_compute_totals();
    test_compute_utilization();
    test_compute_region_bounds();
    test_compute_largest_free_block();
    test_null_safety();
    printf("All flashsummary tests passed.\n");
    return 0;
}
