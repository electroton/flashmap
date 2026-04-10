#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashregion.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_init_zeroes_map(void) {
    FlashMap map;
    /* dirty the memory first */
    memset(&map, 0xFF, sizeof(map));
    flashmap_init(&map);
    assert(map.count == 0);
}

static void test_add_region_basic(void) {
    FlashMap map;
    flashmap_init(&map);
    int rc = flashmap_add_region(&map, "FLASH", 0x08000000, 0x80000);
    assert(rc == 0);
    assert(map.count == 1);
    assert(strcmp(map.regions[0].name, "FLASH") == 0);
    assert(map.regions[0].origin == 0x08000000);
    assert(map.regions[0].length == 0x80000);
    assert(map.regions[0].used == 0);
}

static void test_add_multiple_regions(void) {
    FlashMap map;
    flashmap_init(&map);
    assert(flashmap_add_region(&map, "FLASH", 0x08000000, 0x80000) == 0);
    assert(flashmap_add_region(&map, "RAM",   0x20000000, 0x20000) == 0);
    assert(map.count == 2);
}

static void test_add_region_null_name_fails(void) {
    FlashMap map;
    flashmap_init(&map);
    assert(flashmap_add_region(&map, NULL, 0x08000000, 0x80000) == -1);
    assert(map.count == 0);
}

static void test_find_existing_region(void) {
    FlashMap map;
    flashmap_init(&map);
    flashmap_add_region(&map, "FLASH", 0x08000000, 0x80000);
    FlashRegion *r = flashmap_find(&map, "FLASH");
    assert(r != NULL);
    assert(r->origin == 0x08000000);
}

static void test_find_missing_region_returns_null(void) {
    FlashMap map;
    flashmap_init(&map);
    assert(flashmap_find(&map, "EEPROM") == NULL);
}

static void test_set_used_and_free(void) {
    FlashMap map;
    flashmap_init(&map);
    flashmap_add_region(&map, "FLASH", 0x08000000, 0x80000);
    assert(flashmap_set_used(&map, "FLASH", 0x10000) == 0);
    FlashRegion *r = flashmap_find(&map, "FLASH");
    assert(r != NULL);
    assert(r->used == 0x10000);
    assert(flashmap_free(r) == 0x80000 - 0x10000);
}

static void test_set_used_missing_region_fails(void) {
    FlashMap map;
    flashmap_init(&map);
    assert(flashmap_set_used(&map, "GHOST", 100) == -1);
}

int main(void) {
    RUN_TEST(test_init_zeroes_map);
    RUN_TEST(test_add_region_basic);
    RUN_TEST(test_add_multiple_regions);
    RUN_TEST(test_add_region_null_name_fails);
    RUN_TEST(test_find_existing_region);
    RUN_TEST(test_find_missing_region_returns_null);
    RUN_TEST(test_set_used_and_free);
    RUN_TEST(test_set_used_missing_region_fails);
    printf("flashregion: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
