/*
 * test_flashalias.c - Unit tests for flashalias module
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashalias.h"

static void test_init(void) {
    FlashAliasMap map;
    flashalias_init(&map);
    assert(flashalias_count(&map) == 0);
    printf("  [PASS] test_init\n");
}

static void test_add_and_resolve(void) {
    FlashAliasMap map;
    flashalias_init(&map);

    int rc = flashalias_add(&map, "FLASH_BANK0", "fw_region");
    assert(rc == FLASHALIAS_OK);
    assert(flashalias_count(&map) == 1);

    const char *resolved = flashalias_resolve(&map, "fw_region");
    assert(resolved != NULL);
    assert(strcmp(resolved, "FLASH_BANK0") == 0);
    printf("  [PASS] test_add_and_resolve\n");
}

static void test_resolve_unknown_returns_self(void) {
    FlashAliasMap map;
    flashalias_init(&map);

    const char *resolved = flashalias_resolve(&map, "unknown_region");
    assert(resolved != NULL);
    assert(strcmp(resolved, "unknown_region") == 0);
    printf("  [PASS] test_resolve_unknown_returns_self\n");
}

static void test_duplicate_alias_rejected(void) {
    FlashAliasMap map;
    flashalias_init(&map);

    flashalias_add(&map, "FLASH_BANK0", "fw_region");
    int rc = flashalias_add(&map, "FLASH_BANK1", "fw_region");
    assert(rc == FLASHALIAS_ERR_DUPLICATE);
    assert(flashalias_count(&map) == 1);
    printf("  [PASS] test_duplicate_alias_rejected\n");
}

static void test_remove(void) {
    FlashAliasMap map;
    flashalias_init(&map);

    flashalias_add(&map, "FLASH_BANK0", "fw_region");
    flashalias_add(&map, "FLASH_BANK1", "cfg_region");
    assert(flashalias_count(&map) == 2);

    int rc = flashalias_remove(&map, "fw_region");
    assert(rc == FLASHALIAS_OK);
    assert(flashalias_count(&map) == 1);

    /* Resolved after removal should return the name itself */
    const char *resolved = flashalias_resolve(&map, "fw_region");
    assert(strcmp(resolved, "fw_region") == 0);
    printf("  [PASS] test_remove\n");
}

static void test_remove_not_found(void) {
    FlashAliasMap map;
    flashalias_init(&map);

    int rc = flashalias_remove(&map, "nonexistent");
    assert(rc == FLASHALIAS_ERR_NOT_FOUND);
    printf("  [PASS] test_remove_not_found\n");
}

static void test_null_safety(void) {
    assert(flashalias_add(NULL, "orig", "alias") == FLASHALIAS_ERR_NULL);
    assert(flashalias_resolve(NULL, "alias") == NULL);
    assert(flashalias_remove(NULL, "alias") == FLASHALIAS_ERR_NULL);
    assert(flashalias_count(NULL) == 0);
    printf("  [PASS] test_null_safety\n");
}

static void test_multiple_aliases(void) {
    FlashAliasMap map;
    flashalias_init(&map);

    flashalias_add(&map, "FLASH_BANK0", "boot");
    flashalias_add(&map, "FLASH_BANK1", "app");
    flashalias_add(&map, "FLASH_BANK2", "data");
    assert(flashalias_count(&map) == 3);

    assert(strcmp(flashalias_resolve(&map, "boot"), "FLASH_BANK0") == 0);
    assert(strcmp(flashalias_resolve(&map, "app"),  "FLASH_BANK1") == 0);
    assert(strcmp(flashalias_resolve(&map, "data"), "FLASH_BANK2") == 0);
    printf("  [PASS] test_multiple_aliases\n");
}

int main(void) {
    printf("Running flashalias tests...\n");
    test_init();
    test_add_and_resolve();
    test_resolve_unknown_returns_self();
    test_duplicate_alias_rejected();
    test_remove();
    test_remove_not_found();
    test_null_safety();
    test_multiple_aliases();
    printf("All flashalias tests passed.\n");
    return 0;
}
