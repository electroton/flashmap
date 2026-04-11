#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashtag.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_init_empty(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    assert(ts.count == 0);
}

static void test_add_and_has(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    assert(flashtag_add(&ts, "bootloader") == 1);
    assert(ts.count == 1);
    assert(flashtag_has(&ts, "bootloader") == 1);
    assert(flashtag_has(&ts, "application") == 0);
}

static void test_add_duplicate(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    assert(flashtag_add(&ts, "critical") == 1);
    assert(flashtag_add(&ts, "critical") == 0);
    assert(ts.count == 1);
}

static void test_remove_existing(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    flashtag_add(&ts, "nvs");
    flashtag_add(&ts, "ota");
    assert(flashtag_remove(&ts, "nvs") == 1);
    assert(ts.count == 1);
    assert(flashtag_has(&ts, "nvs") == 0);
    assert(flashtag_has(&ts, "ota") == 1);
}

static void test_remove_nonexistent(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    flashtag_add(&ts, "factory");
    assert(flashtag_remove(&ts, "missing") == 0);
    assert(ts.count == 1);
}

static void test_intersects(void) {
    FlashTagSet a, b;
    flashtag_init(&a);
    flashtag_init(&b);
    flashtag_add(&a, "read-only");
    flashtag_add(&a, "secure");
    flashtag_add(&b, "secure");
    flashtag_add(&b, "encrypted");
    assert(flashtag_intersects(&a, &b) == 1);

    FlashTagSet c;
    flashtag_init(&c);
    flashtag_add(&c, "scratch");
    assert(flashtag_intersects(&a, &c) == 0);
}

static void test_clear(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    flashtag_add(&ts, "tag1");
    flashtag_add(&ts, "tag2");
    flashtag_clear(&ts);
    assert(ts.count == 0);
    assert(flashtag_has(&ts, "tag1") == 0);
}

static void test_max_capacity(void) {
    FlashTagSet ts;
    flashtag_init(&ts);
    char tag[FLASH_TAG_MAX_LEN];
    for (int i = 0; i < FLASH_TAG_MAX_COUNT; i++) {
        snprintf(tag, sizeof(tag), "tag%d", i);
        assert(flashtag_add(&ts, tag) == 1);
    }
    assert(ts.count == FLASH_TAG_MAX_COUNT);
    assert(flashtag_add(&ts, "overflow") == 0);
}

int main(void) {
    RUN_TEST(test_init_empty);
    RUN_TEST(test_add_and_has);
    RUN_TEST(test_add_duplicate);
    RUN_TEST(test_remove_existing);
    RUN_TEST(test_remove_nonexistent);
    RUN_TEST(test_intersects);
    RUN_TEST(test_clear);
    RUN_TEST(test_max_capacity);
    printf("flashtag: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
