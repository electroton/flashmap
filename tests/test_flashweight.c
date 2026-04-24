#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "../src/flashweight.h"
#include "../src/flashweight_report.h"

static FlashLayout *make_layout(void) {
    FlashLayout *layout = calloc(1, sizeof(FlashLayout));
    layout->count = 3;
    layout->regions = calloc(3, sizeof(FlashRegion));

    strncpy(layout->regions[0].name, "bootloader", 63);
    layout->regions[0].address = 0x08000000;
    layout->regions[0].size    = 0x4000;  /* 16 KB */

    strncpy(layout->regions[1].name, "application", 63);
    layout->regions[1].address = 0x08004000;
    layout->regions[1].size    = 0x18000; /* 96 KB */

    strncpy(layout->regions[2].name, "config", 63);
    layout->regions[2].address = 0x0801C000;
    layout->regions[2].size    = 0x4000;  /* 16 KB */

    return layout;
}

static void free_layout(FlashLayout *layout) {
    free(layout->regions);
    free(layout);
}

static void test_compute_basic(void) {
    FlashLayout *layout = make_layout();
    FlashWeightResult *result = flash_weight_compute(layout);
    assert(result != NULL);
    assert(result->count == 3);
    /* total = 16+96+16 = 128 KB */
    assert(result->total_size == 0x20000);

    /* application should be 96/128 = 0.75 */
    assert(fabs(result->entries[1].weight - 0.75) < 1e-9);
    assert(fabs(result->entries[1].weight_pct - 75.0) < 1e-6);

    flash_weight_result_free(result);
    free_layout(layout);
    printf("PASS test_compute_basic\n");
}

static void test_heaviest_lightest(void) {
    FlashLayout *layout = make_layout();
    FlashWeightResult *result = flash_weight_compute(layout);
    assert(result != NULL);

    const FlashWeightEntry *heavy = flash_weight_heaviest(result);
    assert(heavy != NULL);
    assert(strcmp(heavy->name, "application") == 0);

    const FlashWeightEntry *light = flash_weight_lightest(result);
    assert(light != NULL);
    /* bootloader and config are equal; either may be returned */
    assert(light->size == 0x4000);

    flash_weight_result_free(result);
    free_layout(layout);
    printf("PASS test_heaviest_lightest\n");
}

static void test_entropy_positive(void) {
    FlashLayout *layout = make_layout();
    FlashWeightResult *result = flash_weight_compute(layout);
    assert(result != NULL);
    assert(result->entropy > 0.0);
    flash_weight_result_free(result);
    free_layout(layout);
    printf("PASS test_entropy_positive\n");
}

static void test_null_inputs(void) {
    assert(flash_weight_compute(NULL) == NULL);
    assert(flash_weight_heaviest(NULL) == NULL);
    assert(flash_weight_lightest(NULL) == NULL);
    assert(fabs(flash_weight_entropy(NULL)) < 1e-12);
    printf("PASS test_null_inputs\n");
}

static void test_report_smoke(void) {
    FlashLayout *layout = make_layout();
    FlashWeightResult *result = flash_weight_compute(layout);
    assert(result != NULL);
    flash_weight_report_print(result, stdout);
    flash_weight_report_print_summary(result, stdout);
    flash_weight_result_free(result);
    free_layout(layout);
    printf("PASS test_report_smoke\n");
}

int main(void) {
    test_compute_basic();
    test_heaviest_lightest();
    test_entropy_positive();
    test_null_inputs();
    test_report_smoke();
    printf("All flashweight tests passed.\n");
    return 0;
}
