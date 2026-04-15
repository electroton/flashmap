#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashwatermark.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { \
    printf("  %-45s", #fn); \
    fn(); \
    tests_run++; \
    tests_passed++; \
    printf("PASS\n"); \
} while (0)

static void test_init_and_free(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);
    assert(set.count    == 0);
    assert(set.capacity == 0);
    assert(set.marks    == NULL);
    flash_watermark_set_free(&set);
    assert(set.marks == NULL);
}

static void test_record_new_entry(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);

    int rc = flash_watermark_record(&set, "FLASH", 0x08000000, 0x80000, 0x10000);
    assert(rc == 0);
    assert(set.count == 1);
    assert(set.marks[0].peak_used    == 0x10000);
    assert(set.marks[0].current_used == 0x10000);
    assert(set.marks[0].base         == 0x08000000);
    assert(set.marks[0].capacity     == 0x80000);

    flash_watermark_set_free(&set);
}

static void test_record_updates_peak(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);

    flash_watermark_record(&set, "ROM", 0x0, 0x40000, 0x8000);
    flash_watermark_record(&set, "ROM", 0x0, 0x40000, 0x20000); /* higher */
    flash_watermark_record(&set, "ROM", 0x0, 0x40000, 0x5000);  /* lower  */

    assert(set.count == 1);
    assert(set.marks[0].peak_used    == 0x20000);
    assert(set.marks[0].current_used == 0x5000);

    flash_watermark_set_free(&set);
}

static void test_peak_percentage(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);

    flash_watermark_record(&set, "EEPROM", 0x0, 0x1000, 0x800);
    const FlashWatermark *m = flash_watermark_find(&set, "EEPROM");
    assert(m != NULL);
    /* 0x800 / 0x1000 = 50% */
    assert(m->peak_pct > 49.9f && m->peak_pct < 50.1f);

    flash_watermark_set_free(&set);
}

static void test_find_missing(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);

    flash_watermark_record(&set, "A", 0x0, 0x1000, 0x100);
    const FlashWatermark *m = flash_watermark_find(&set, "B");
    assert(m == NULL);

    flash_watermark_set_free(&set);
}

static void test_reset_peaks(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);

    flash_watermark_record(&set, "SRAM", 0x20000000, 0x8000, 0x4000);
    flash_watermark_record(&set, "SRAM", 0x20000000, 0x8000, 0x1000);
    assert(set.marks[0].peak_used == 0x4000);

    flash_watermark_reset_peaks(&set);
    assert(set.marks[0].peak_used == 0x1000); /* reset to current */

    flash_watermark_set_free(&set);
}

static void test_multiple_regions(void) {
    FlashWatermarkSet set;
    flash_watermark_set_init(&set);

    for (int i = 0; i < 10; i++) {
        char name[16];
        snprintf(name, sizeof(name), "region%d", i);
        flash_watermark_record(&set, name, (uint32_t)(i * 0x10000),
                               0x10000, (uint32_t)(i * 0x800));
    }
    assert(set.count == 10);

    const FlashWatermark *m = flash_watermark_find(&set, "region5");
    assert(m != NULL);
    assert(m->peak_used == 5 * 0x800);

    flash_watermark_set_free(&set);
}

int main(void) {
    printf("=== test_flashwatermark ===\n");
    RUN_TEST(test_init_and_free);
    RUN_TEST(test_record_new_entry);
    RUN_TEST(test_record_updates_peak);
    RUN_TEST(test_peak_percentage);
    RUN_TEST(test_find_missing);
    RUN_TEST(test_reset_peaks);
    RUN_TEST(test_multiple_regions);
    printf("\nResults: %d/%d tests passed.\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? EXIT_SUCCESS : EXIT_FAILURE;
}
