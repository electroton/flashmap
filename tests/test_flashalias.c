#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashalias.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  [TEST] %s\n", name); } while (0)
#define PASS()     do { tests_passed++; } while (0)
#define FAIL(msg)  do { printf("  [FAIL] %s\n", msg); } while (0)

static void test_init_free(void) {
    TEST("init and free");
    FlashAliasTable t;
    flashalias_init(&t);
    assert(t.count == 0);
    flashalias_free(&t);
    PASS();
}

static void test_add_and_resolve(void) {
    TEST("add alias and resolve");
    FlashAliasTable t;
    flashalias_init(&t);

    int rc = flashalias_add(&t, "FLASH_APP", "app");
    assert(rc == 0);
    rc = flashalias_add(&t, "FLASH_APP", "application");
    assert(rc == 0);

    const char *resolved = flashalias_resolve(&t, "app");
    assert(resolved != NULL);
    assert(strcmp(resolved, "FLASH_APP") == 0);

    resolved = flashalias_resolve(&t, "application");
    assert(resolved != NULL);
    assert(strcmp(resolved, "FLASH_APP") == 0);

    flashalias_free(&t);
    PASS();
}

static void test_resolve_by_region_name(void) {
    TEST("resolve by canonical region name");
    FlashAliasTable t;
    flashalias_init(&t);
    flashalias_add(&t, "BOOTLOADER", "boot");

    /* The region name itself should resolve to itself */
    const char *resolved = flashalias_resolve(&t, "BOOTLOADER");
    assert(resolved != NULL);
    assert(strcmp(resolved, "BOOTLOADER") == 0);

    flashalias_free(&t);
    PASS();
}

static void test_resolve_unknown_returns_null(void) {
    TEST("resolve unknown alias returns NULL");
    FlashAliasTable t;
    flashalias_init(&t);
    assert(flashalias_resolve(&t, "nonexistent") == NULL);
    flashalias_free(&t);
    PASS();
}

static void test_duplicate_alias_rejected(void) {
    TEST("duplicate alias is rejected");
    FlashAliasTable t;
    flashalias_init(&t);
    flashalias_add(&t, "REGION_A", "ra");
    int rc = flashalias_add(&t, "REGION_B", "ra");
    assert(rc == -1);
    flashalias_free(&t);
    PASS();
}

static void test_remove(void) {
    TEST("remove aliases for a region");
    FlashAliasTable t;
    flashalias_init(&t);
    flashalias_add(&t, "NVRAM", "nv");
    flashalias_add(&t, "NVRAM", "nvram_alt");

    int removed = flashalias_remove(&t, "NVRAM");
    assert(removed == 1);
    assert(flashalias_resolve(&t, "nv") == NULL);
    assert(flashalias_resolve(&t, "nvram_alt") == NULL);

    flashalias_free(&t);
    PASS();
}

static void test_list(void) {
    TEST("list aliases for a region");
    FlashAliasTable t;
    flashalias_init(&t);
    flashalias_add(&t, "OTA", "ota_slot0");
    flashalias_add(&t, "OTA", "ota_primary");

    char out[FLASHALIAS_MAX_ALIASES][FLASHALIAS_NAME_LEN];
    size_t n = flashalias_list(&t, "OTA", out, FLASHALIAS_MAX_ALIASES);
    assert(n == 2);

    flashalias_free(&t);
    PASS();
}

int main(void) {
    printf("Running flashalias tests...\n");
    test_init_free();
    test_add_and_resolve();
    test_resolve_by_region_name();
    test_resolve_unknown_returns_null();
    test_duplicate_alias_rejected();
    test_remove();
    test_list();
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
