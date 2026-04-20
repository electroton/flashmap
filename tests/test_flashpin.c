#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashpin.h"
#include "../src/flashpin_report.h"

static void test_create_destroy(void) {
    FlashPinSet *ps = flashpin_create();
    assert(ps != NULL);
    assert(flashpin_count(ps) == 0);
    flashpin_destroy(ps);
    printf("[PASS] test_create_destroy\n");
}

static void test_add_and_find(void) {
    FlashPinSet *ps = flashpin_create();
    assert(flashpin_add(ps, "reset_vector", 0x08000000, FLASH_PIN_ENTRY) == 0);
    assert(flashpin_add(ps, "end_of_flash", 0x080FFFFF, FLASH_PIN_BOUNDARY) == 0);
    assert(flashpin_count(ps) == 2);

    FlashPin *p = flashpin_find(ps, "reset_vector");
    assert(p != NULL);
    assert(p->address == 0x08000000);
    assert(p->type == FLASH_PIN_ENTRY);
    assert(!p->locked);

    FlashPin *p2 = flashpin_find_by_address(ps, 0x080FFFFF);
    assert(p2 != NULL);
    assert(strcmp(p2->name, "end_of_flash") == 0);

    flashpin_destroy(ps);
    printf("[PASS] test_add_and_find\n");
}

static void test_remove(void) {
    FlashPinSet *ps = flashpin_create();
    flashpin_add(ps, "marker_a", 0x1000, FLASH_PIN_MARKER);
    flashpin_add(ps, "marker_b", 0x2000, FLASH_PIN_MARKER);
    assert(flashpin_remove(ps, "marker_a") == 0);
    assert(flashpin_count(ps) == 1);
    assert(flashpin_find(ps, "marker_a") == NULL);
    assert(flashpin_find(ps, "marker_b") != NULL);
    assert(flashpin_remove(ps, "nonexistent") == -1);
    flashpin_destroy(ps);
    printf("[PASS] test_remove\n");
}

static void test_lock_unlock(void) {
    FlashPinSet *ps = flashpin_create();
    flashpin_add(ps, "locked_pin", 0x3000, FLASH_PIN_ENTRY);
    assert(flashpin_lock(ps, "locked_pin") == 0);
    FlashPin *p = flashpin_find(ps, "locked_pin");
    assert(p->locked == true);
    assert(flashpin_remove(ps, "locked_pin") == -2);
    assert(flashpin_unlock(ps, "locked_pin") == 0);
    assert(p->locked == false);
    assert(flashpin_remove(ps, "locked_pin") == 0);
    flashpin_destroy(ps);
    printf("[PASS] test_lock_unlock\n");
}

static void test_report(void) {
    FlashPinSet *ps = flashpin_create();
    flashpin_add(ps, "entry_point",  0x08000000, FLASH_PIN_ENTRY);
    flashpin_add(ps, "sector_end",   0x0800FFFF, FLASH_PIN_BOUNDARY);
    flashpin_add(ps, "debug_marker", 0x08004000, FLASH_PIN_MARKER);
    flashpin_lock(ps, "entry_point");
    flashpin_report_print(ps, stdout);
    flashpin_destroy(ps);
    printf("[PASS] test_report\n");
}

static void test_null_safety(void) {
    assert(flashpin_add(NULL, "x", 0, FLASH_PIN_MARKER) == -1);
    assert(flashpin_find(NULL, "x") == NULL);
    assert(flashpin_find_by_address(NULL, 0) == NULL);
    assert(flashpin_remove(NULL, "x") == -1);
    assert(flashpin_count(NULL) == 0);
    flashpin_report_print(NULL, stdout);
    printf("[PASS] test_null_safety\n");
}

int main(void) {
    test_create_destroy();
    test_add_and_find();
    test_remove();
    test_lock_unlock();
    test_report();
    test_null_safety();
    printf("All flashpin tests passed.\n");
    return 0;
}
