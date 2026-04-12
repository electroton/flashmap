#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashindex.h"

static void test_create_destroy(void) {
    FlashIndex *idx = flash_index_create();
    assert(idx != NULL);
    assert(flash_index_count(idx) == 0);
    flash_index_destroy(idx);
    printf("[PASS] test_create_destroy\n");
}

static void test_insert_and_count(void) {
    FlashIndex *idx = flash_index_create();
    assert(flash_index_insert(idx, "bootloader", 0x08000000, 0x4000) == 0);
    assert(flash_index_insert(idx, "application", 0x08004000, 0x1C000) == 0);
    assert(flash_index_count(idx) == 2);
    flash_index_destroy(idx);
    printf("[PASS] test_insert_and_count\n");
}

static void test_find_by_name_found(void) {
    FlashIndex *idx = flash_index_create();
    flash_index_insert(idx, "config", 0x08020000, 0x1000);
    const FlashIndexEntry *e = flash_index_find_by_name(idx, "config");
    assert(e != NULL);
    assert(e->address == 0x08020000);
    assert(e->size == 0x1000);
    flash_index_destroy(idx);
    printf("[PASS] test_find_by_name_found\n");
}

static void test_find_by_name_not_found(void) {
    FlashIndex *idx = flash_index_create();
    flash_index_insert(idx, "region_a", 0x08000000, 0x2000);
    const FlashIndexEntry *e = flash_index_find_by_name(idx, "region_b");
    assert(e == NULL);
    flash_index_destroy(idx);
    printf("[PASS] test_find_by_name_not_found\n");
}

static void test_find_by_address_in_range(void) {
    FlashIndex *idx = flash_index_create();
    flash_index_insert(idx, "firmware", 0x08008000, 0x8000);
    const FlashIndexEntry *e = flash_index_find_by_address(idx, 0x08009000);
    assert(e != NULL);
    assert(strcmp(e->name, "firmware") == 0);
    flash_index_destroy(idx);
    printf("[PASS] test_find_by_address_in_range\n");
}

static void test_find_by_address_out_of_range(void) {
    FlashIndex *idx = flash_index_create();
    flash_index_insert(idx, "firmware", 0x08008000, 0x8000);
    const FlashIndexEntry *e = flash_index_find_by_address(idx, 0x08000000);
    assert(e == NULL);
    flash_index_destroy(idx);
    printf("[PASS] test_find_by_address_out_of_range\n");
}

static void test_clear(void) {
    FlashIndex *idx = flash_index_create();
    flash_index_insert(idx, "a", 0x1000, 0x100);
    flash_index_insert(idx, "b", 0x2000, 0x200);
    flash_index_clear(idx);
    assert(flash_index_count(idx) == 0);
    assert(flash_index_find_by_name(idx, "a") == NULL);
    flash_index_destroy(idx);
    printf("[PASS] test_clear\n");
}

static void test_null_safety(void) {
    assert(flash_index_find_by_name(NULL, "x") == NULL);
    assert(flash_index_find_by_address(NULL, 0x0) == NULL);
    assert(flash_index_count(NULL) == 0);
    flash_index_destroy(NULL);
    printf("[PASS] test_null_safety\n");
}

int main(void) {
    test_create_destroy();
    test_insert_and_count();
    test_find_by_name_found();
    test_find_by_name_not_found();
    test_find_by_address_in_range();
    test_find_by_address_out_of_range();
    test_clear();
    test_null_safety();
    printf("All flashindex tests passed.\n");
    return 0;
}
