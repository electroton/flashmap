/*
 * tests/test_flashindex.c
 * Unit tests for flashindex and flashindex_report modules.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashindex.h"
#include "../src/flashindex_report.h"

static void test_flashindex_init(void) {
    FlashIndex idx;
    flashindex_init(&idx);
    assert(idx.count == 0);
    printf("[PASS] test_flashindex_init\n");
}

static void test_flashindex_add_and_find(void) {
    FlashIndex idx;
    flashindex_init(&idx);

    int r = flashindex_add(&idx, "bootloader", 0x08000000, 0x8000);
    assert(r == 0);
    assert(idx.count == 1);

    r = flashindex_add(&idx, "application", 0x08008000, 0x30000);
    assert(r == 0);
    assert(idx.count == 2);

    const FlashIndexEntry *e = flashindex_find(&idx, "bootloader");
    assert(e != NULL);
    assert(e->start == 0x08000000);
    assert(e->size  == 0x8000);

    e = flashindex_find(&idx, "application");
    assert(e != NULL);
    assert(e->start == 0x08008000);

    e = flashindex_find(&idx, "nonexistent");
    assert(e == NULL);

    printf("[PASS] test_flashindex_add_and_find\n");
}

static void test_flashindex_add_duplicate(void) {
    FlashIndex idx;
    flashindex_init(&idx);

    flashindex_add(&idx, "region", 0x1000, 0x400);
    int r = flashindex_add(&idx, "region", 0x2000, 0x400);
    /* Duplicate names should be rejected */
    assert(r != 0);
    assert(idx.count == 1);

    printf("[PASS] test_flashindex_add_duplicate\n");
}

static void test_flashindex_remove(void) {
    FlashIndex idx;
    flashindex_init(&idx);

    flashindex_add(&idx, "alpha", 0x0000, 0x100);
    flashindex_add(&idx, "beta",  0x0200, 0x100);
    assert(idx.count == 2);

    int r = flashindex_remove(&idx, "alpha");
    assert(r == 0);
    assert(idx.count == 1);
    assert(flashindex_find(&idx, "alpha") == NULL);
    assert(flashindex_find(&idx, "beta")  != NULL);

    r = flashindex_remove(&idx, "missing");
    assert(r != 0);

    printf("[PASS] test_flashindex_remove\n");
}

static void test_flashindex_report(void) {
    FlashIndex idx;
    flashindex_init(&idx);

    flashindex_add(&idx, "bootloader",  0x08000000, 0x8000);
    flashindex_add(&idx, "application", 0x08008000, 0x30000);
    flashindex_add(&idx, "config",      0x08038000, 0x1000);

    /* Should not crash with valid inputs */
    flashindex_report_print(&idx, stdout);
    flashindex_report_summary(&idx, stdout);

    /* Should not crash with NULL inputs */
    flashindex_report_print(NULL, stdout);
    flashindex_report_summary(NULL, stdout);

    printf("[PASS] test_flashindex_report\n");
}

int main(void) {
    test_flashindex_init();
    test_flashindex_add_and_find();
    test_flashindex_add_duplicate();
    test_flashindex_remove();
    test_flashindex_report();

    printf("\nAll flashindex tests passed.\n");
    return 0;
}
