#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashtag.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while(0)

static void test_init_zeroes_registry(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    assert(reg.count == 0);
}

static void test_add_single_tag(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    int ret = flashtag_add(&reg, "FLASH", "bootable");
    assert(ret == FLASHTAG_OK);
    assert(reg.count == 1);
    assert(strcmp(reg.tags[0].region_name, "FLASH") == 0);
    assert(strcmp(reg.tags[0].tag, "bootable") == 0);
}

static void test_add_duplicate_returns_error(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    flashtag_add(&reg, "FLASH", "bootable");
    int ret = flashtag_add(&reg, "FLASH", "bootable");
    assert(ret == FLASHTAG_ERR_DUPLICATE);
    assert(reg.count == 1);
}

static void test_add_multiple_tags_same_region(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    assert(flashtag_add(&reg, "FLASH", "bootable") == FLASHTAG_OK);
    assert(flashtag_add(&reg, "FLASH", "read-only") == FLASHTAG_OK);
    assert(reg.count == 2);
}

static void test_has_tag_returns_true(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    flashtag_add(&reg, "RAM", "volatile");
    assert(flashtag_has_tag(&reg, "RAM", "volatile") == 1);
}

static void test_has_tag_returns_false_for_missing(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    flashtag_add(&reg, "RAM", "volatile");
    assert(flashtag_has_tag(&reg, "RAM", "persistent") == 0);
    assert(flashtag_has_tag(&reg, "FLASH", "volatile") == 0);
}

static void test_remove_existing_tag(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    flashtag_add(&reg, "FLASH", "bootable");
    int ret = flashtag_remove(&reg, "FLASH", "bootable");
    assert(ret == FLASHTAG_OK);
    assert(reg.count == 0);
    assert(flashtag_has_tag(&reg, "FLASH", "bootable") == 0);
}

static void test_remove_nonexistent_tag(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    int ret = flashtag_remove(&reg, "FLASH", "bootable");
    assert(ret == FLASHTAG_ERR_NOT_FOUND);
}

static void test_get_tags_for_region(void) {
    FlashTagRegistry reg;
    flashtag_init(&reg);
    flashtag_add(&reg, "FLASH", "bootable");
    flashtag_add(&reg, "FLASH", "read-only");
    flashtag_add(&reg, "RAM", "volatile");

    char out[8][FLASHTAG_TAG_LEN];
    int count = flashtag_get_tags_for_region(&reg, "FLASH", out, 8);
    assert(count == 2);
}

static void test_null_inputs_handled(void) {
    assert(flashtag_add(NULL, "FLASH", "tag") == FLASHTAG_ERR_NULL);
    assert(flashtag_has_tag(NULL, "FLASH", "tag") == 0);
    assert(flashtag_remove(NULL, "FLASH", "tag") == FLASHTAG_ERR_NULL);
    assert(flashtag_get_tags_for_region(NULL, "FLASH", NULL, 0) == 0);
}

int main(void) {
    RUN_TEST(test_init_zeroes_registry);
    RUN_TEST(test_add_single_tag);
    RUN_TEST(test_add_duplicate_returns_error);
    RUN_TEST(test_add_multiple_tags_same_region);
    RUN_TEST(test_has_tag_returns_true);
    RUN_TEST(test_has_tag_returns_false_for_missing);
    RUN_TEST(test_remove_existing_tag);
    RUN_TEST(test_remove_nonexistent_tag);
    RUN_TEST(test_get_tags_for_region);
    RUN_TEST(test_null_inputs_handled);
    printf("flashtag tests: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
