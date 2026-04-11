#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/flashsummary.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* Helper: build a small layout with known regions */
static FlashLayout build_test_layout(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));

    /* Region 0: 50% used */
    strncpy(layout.regions[0].name, "FLASH", REGION_NAME_MAX);
    layout.regions[0].start   = 0x08000000;
    layout.regions[0].size    = 512 * 1024;
    layout.regions[0].used    = 256 * 1024;

    /* Region 1: 95% used (critical) */
    strncpy(layout.regions[1].name, "EEPROM", REGION_NAME_MAX);
    layout.regions[1].start   = 0x08080000;
    layout.regions[1].size    = 64 * 1024;
    layout.regions[1].used    = (size_t)(64 * 1024 * 0.95);

    /* Region 2: 100% full */
    strncpy(layout.regions[2].name, "BOOTLOADER", REGION_NAME_MAX);
    layout.regions[2].start   = 0x08100000;
    layout.regions[2].size    = 32 * 1024;
    layout.regions[2].used    = 32 * 1024;

    layout.region_count = 3;
    return layout;
}

static void test_compute_null_inputs(void) {
    FlashSummary summary;
    FlashLayout  layout;
    assert(flash_summary_compute(NULL, &summary) == -1);
    assert(flash_summary_compute(&layout, NULL)  == -1);
    printf("PASS: test_compute_null_inputs\n");
}

static void test_compute_totals(void) {
    FlashLayout  layout  = build_test_layout();
    FlashSummary summary;

    assert(flash_summary_compute(&layout, &summary) == 0);
    assert(summary.region_count   == 3);
    assert(summary.total_capacity == 512*1024 + 64*1024 + 32*1024);
    assert(summary.total_used     == 256*1024 + (size_t)(64*1024*0.95) + 32*1024);
    assert(summary.total_free     == summary.total_capacity - summary.total_used);
    printf("PASS: test_compute_totals\n");
}

static void test_compute_critical_and_full(void) {
    FlashLayout  layout  = build_test_layout();
    FlashSummary summary;

    assert(flash_summary_compute(&layout, &summary) == 0);
    assert(summary.full_regions     == 1); /* BOOTLOADER */
    assert(summary.critical_regions == 2); /* EEPROM + BOOTLOADER */
    printf("PASS: test_compute_critical_and_full\n");
}

static void test_compute_most_least_used(void) {
    FlashLayout  layout  = build_test_layout();
    FlashSummary summary;

    assert(flash_summary_compute(&layout, &summary) == 0);
    assert(summary.most_used_region  != NULL);
    assert(summary.least_used_region != NULL);
    assert(strcmp(summary.most_used_region->name,  "BOOTLOADER") == 0);
    assert(strcmp(summary.least_used_region->name, "FLASH")      == 0);
    printf("PASS: test_compute_most_least_used\n");
}

static void test_usage_percent_range(void) {
    FlashLayout  layout  = build_test_layout();
    FlashSummary summary;

    assert(flash_summary_compute(&layout, &summary) == 0);
    assert(summary.usage_percent >= 0.0 && summary.usage_percent <= 100.0);
    printf("PASS: test_usage_percent_range\n");
}

int main(void) {
    test_compute_null_inputs();
    test_compute_totals();
    test_compute_critical_and_full();
    test_compute_most_least_used();
    test_usage_percent_range();
    printf("All flashsummary tests passed.\n");
    return 0;
}
