#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashthreshold.h"
#include "../src/flashthreshold_report.h"

static FlashRegion make_region(const char *name, uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.size      = size;
    r.used_size = used;
    return r;
}

static void test_context_create_free(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    assert(ctx != NULL);
    assert(ctx->count == 0);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_context_create_free\n");
}

static void test_add_config(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    assert(flash_threshold_add(ctx, "FLASH", 70, 90) == 0);
    assert(ctx->count == 1);
    assert(strcmp(ctx->configs[0].region_name, "FLASH") == 0);
    assert(ctx->configs[0].warning_percent  == 70);
    assert(ctx->configs[0].critical_percent == 90);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_add_config\n");
}

static void test_add_invalid_percent(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    /* warning >= critical should fail */
    assert(flash_threshold_add(ctx, "FLASH", 90, 70) == -1);
    /* over 100 should fail */
    assert(flash_threshold_add(ctx, "FLASH", 70, 110) == -1);
    assert(ctx->count == 0);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_add_invalid_percent\n");
}

static void test_evaluate_ok(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    flash_threshold_add(ctx, "FLASH", 70, 90);
    FlashRegion r = make_region("FLASH", 1000, 500);  /* 50% */
    FlashThresholdResult res;
    ThresholdLevel lvl = flash_threshold_evaluate(ctx, &r, &res);
    assert(lvl == THRESHOLD_OK);
    assert(res.used_percent == 50);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_evaluate_ok\n");
}

static void test_evaluate_warning(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    flash_threshold_add(ctx, "FLASH", 70, 90);
    FlashRegion r = make_region("FLASH", 1000, 750);  /* 75% */
    FlashThresholdResult res;
    ThresholdLevel lvl = flash_threshold_evaluate(ctx, &r, &res);
    assert(lvl == THRESHOLD_WARNING);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_evaluate_warning\n");
}

static void test_evaluate_critical(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    flash_threshold_add(ctx, "FLASH", 70, 90);
    FlashRegion r = make_region("FLASH", 1000, 950);  /* 95% */
    FlashThresholdResult res;
    ThresholdLevel lvl = flash_threshold_evaluate(ctx, &r, &res);
    assert(lvl == THRESHOLD_CRITICAL);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_evaluate_critical\n");
}

static void test_evaluate_unknown_region(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    flash_threshold_add(ctx, "FLASH", 70, 90);
    FlashRegion r = make_region("RAM", 1000, 999);
    ThresholdLevel lvl = flash_threshold_evaluate(ctx, &r, NULL);
    /* No config for RAM => always OK */
    assert(lvl == THRESHOLD_OK);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_evaluate_unknown_region\n");
}

static void test_evaluate_all(void) {
    FlashThresholdContext *ctx = flash_threshold_context_create();
    flash_threshold_add(ctx, "FLASH", 70, 90);
    flash_threshold_add(ctx, "RAM",   60, 80);

    FlashRegion regions[2];
    regions[0] = make_region("FLASH", 1000, 800);  /* 80% -> WARNING */
    regions[1] = make_region("RAM",   512,  450);  /* 87% -> CRITICAL */

    FlashThresholdResult results[2];
    int n = flash_threshold_evaluate_all(ctx, regions, 2, results, 2);
    assert(n == 2);
    assert(results[0].level == THRESHOLD_WARNING);
    assert(results[1].level == THRESHOLD_CRITICAL);
    flash_threshold_context_free(ctx);
    printf("[PASS] test_evaluate_all\n");
}

static void test_level_str(void) {
    assert(strcmp(flash_threshold_level_str(THRESHOLD_OK),       "OK")       == 0);
    assert(strcmp(flash_threshold_level_str(THRESHOLD_WARNING),  "WARNING")  == 0);
    assert(strcmp(flash_threshold_level_str(THRESHOLD_CRITICAL), "CRITICAL") == 0);
    printf("[PASS] test_level_str\n");
}

static void test_report_print(void) {
    FlashThresholdResult results[2];
    strncpy(results[0].region_name, "FLASH", 63);
    results[0].used_bytes   = 800;
    results[0].total_bytes  = 1000;
    results[0].used_percent = 80;
    results[0].level        = THRESHOLD_WARNING;

    strncpy(results[1].region_name, "RAM", 63);
    results[1].used_bytes   = 450;
    results[1].total_bytes  = 512;
    results[1].used_percent = 87;
    results[1].level        = THRESHOLD_CRITICAL;

    flash_threshold_report_print(results, 2, stdout);
    printf("[PASS] test_report_print\n");
}

int main(void) {
    test_context_create_free();
    test_add_config();
    test_add_invalid_percent();
    test_evaluate_ok();
    test_evaluate_warning();
    test_evaluate_critical();
    test_evaluate_unknown_region();
    test_evaluate_all();
    test_level_str();
    test_report_print();
    printf("All flashthreshold tests passed.\n");
    return 0;
}
