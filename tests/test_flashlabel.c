#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashlabel.h"
#include "../src/flashlabel_report.h"

static void test_init(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    assert(set.count == 0);
    printf("[PASS] test_init\n");
}

static void test_add_and_find(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);

    int r = flashlabel_add(&set, "bootloader", 0x08000000, 0x4000,
                           FLASHLABEL_KIND_SECTION, "Boot ROM");
    assert(r == 0);
    assert(set.count == 1);

    const FlashLabel *l = flashlabel_find(&set, "bootloader");
    assert(l != NULL);
    assert(l->address == 0x08000000);
    assert(l->size    == 0x4000);
    assert(l->kind    == FLASHLABEL_KIND_SECTION);
    assert(strcmp(l->note, "Boot ROM") == 0);
    printf("[PASS] test_add_and_find\n");
}

static void test_duplicate_rejected(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    flashlabel_add(&set, "app", 0x08004000, 0x10000, FLASHLABEL_KIND_SECTION, NULL);
    int r = flashlabel_add(&set, "app", 0x08004000, 0x10000, FLASHLABEL_KIND_SECTION, NULL);
    assert(r == -1);
    assert(set.count == 1);
    printf("[PASS] test_duplicate_rejected\n");
}

static void test_remove(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    flashlabel_add(&set, "tmp", 0x100, 0x10, FLASHLABEL_KIND_CUSTOM, NULL);
    assert(set.count == 1);
    int r = flashlabel_remove(&set, "tmp");
    assert(r == 0);
    assert(set.count == 0);
    assert(flashlabel_find(&set, "tmp") == NULL);
    printf("[PASS] test_remove\n");
}

static void test_pin_prevents_remove(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    flashlabel_add(&set, "critical", 0x200, 0x20, FLASHLABEL_KIND_BOUNDARY, NULL);
    flashlabel_pin(&set, "critical");
    int r = flashlabel_remove(&set, "critical");
    assert(r == -1);
    assert(set.count == 1);
    flashlabel_unpin(&set, "critical");
    r = flashlabel_remove(&set, "critical");
    assert(r == 0);
    printf("[PASS] test_pin_prevents_remove\n");
}

static void test_at_address(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    flashlabel_add(&set, "region_a", 0x1000, 0x500, FLASHLABEL_KIND_SECTION, NULL);
    const FlashLabel *l = flashlabel_at(&set, 0x1200);
    assert(l != NULL);
    assert(strcmp(l->name, "region_a") == 0);
    assert(flashlabel_at(&set, 0x2000) == NULL);
    printf("[PASS] test_at_address\n");
}

static void test_overlapping(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    flashlabel_add(&set, "a", 0x0000, 0x1000, FLASHLABEL_KIND_SECTION, NULL);
    flashlabel_add(&set, "b", 0x0800, 0x1000, FLASHLABEL_KIND_SECTION, NULL);
    flashlabel_add(&set, "c", 0x3000, 0x0100, FLASHLABEL_KIND_SYMBOL,  NULL);

    FlashLabel out[8];
    size_t n = flashlabel_overlapping(&set, 0x0900, 0x0100, out, 8);
    assert(n == 2);
    printf("[PASS] test_overlapping\n");
}

static void test_report(void) {
    FlashLabelSet set;
    flashlabel_set_init(&set);
    flashlabel_add(&set, "boot",  0x08000000, 0x4000,  FLASHLABEL_KIND_SECTION,  "Bootloader");
    flashlabel_add(&set, "app",   0x08004000, 0x10000, FLASHLABEL_KIND_SECTION,  "Application");
    flashlabel_add(&set, "reset", 0x08000004, 0x4,     FLASHLABEL_KIND_SYMBOL,   "Reset vector");
    flashlabel_report_print(&set, stdout);
    flashlabel_report_by_kind(&set, FLASHLABEL_KIND_SYMBOL, stdout);
    printf("[PASS] test_report\n");
}

int main(void) {
    test_init();
    test_add_and_find();
    test_duplicate_rejected();
    test_remove();
    test_pin_prevents_remove();
    test_at_address();
    test_overlapping();
    test_report();
    printf("\nAll flashlabel tests passed.\n");
    return 0;
}
