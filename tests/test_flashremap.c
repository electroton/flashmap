#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashremap.h"
#include "../src/flashregion.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_init(void)
{
    FlashRemapTable t;
    flashremap_init(&t);
    assert(t.count == 0);
}

static void test_add_and_find(void)
{
    FlashRemapTable t;
    flashremap_init(&t);

    int rc = flashremap_add(&t, "bootloader", 0x08000000, 0x00000000, 0x10000);
    assert(rc == 0);
    assert(t.count == 1);

    const FlashRemapEntry *e = flashremap_find(&t, "bootloader");
    assert(e != NULL);
    assert(e->src_base == 0x08000000);
    assert(e->dst_base == 0x00000000);
    assert(e->size     == 0x10000);

    assert(flashremap_find(&t, "nonexistent") == NULL);
}

static void test_translate_hit(void)
{
    FlashRemapTable t;
    flashremap_init(&t);
    flashremap_add(&t, "app", 0x08010000, 0x20000000, 0x8000);

    /* address at start of region */
    uint32_t out = flashremap_translate(&t, 0x08010000);
    assert(out == 0x20000000);

    /* address with offset */
    out = flashremap_translate(&t, 0x08010100);
    assert(out == 0x20000100);

    /* last byte in region */
    out = flashremap_translate(&t, 0x08017FFF);
    assert(out == 0x20007FFF);
}

static void test_translate_miss(void)
{
    FlashRemapTable t;
    flashremap_init(&t);
    flashremap_add(&t, "app", 0x08010000, 0x20000000, 0x8000);

    /* address just outside region end */
    uint32_t out = flashremap_translate(&t, 0x08018000);
    assert(out == 0x08018000);

    /* address before region */
    out = flashremap_translate(&t, 0x0800FFFF);
    assert(out == 0x0800FFFF);
}

static void test_apply_region(void)
{
    FlashRemapTable t;
    flashremap_init(&t);
    flashremap_add(&t, "data", 0x08020000, 0x20010000, 0x4000);

    FlashRegion r;
    memset(&r, 0, sizeof(r));
    r.address = 0x08021000;
    r.size    = 0x200;

    int remapped = flashremap_apply(&t, &r);
    assert(remapped == 1);
    assert(r.address == 0x20011000);

    /* Region outside mapping should not be changed */
    r.address = 0x08030000;
    remapped = flashremap_apply(&t, &r);
    assert(remapped == 0);
    assert(r.address == 0x08030000);
}

static void test_add_invalid(void)
{
    FlashRemapTable t;
    flashremap_init(&t);

    assert(flashremap_add(NULL, "x", 0, 0, 0x100) == -1);
    assert(flashremap_add(&t, NULL, 0, 0, 0x100) == -1);
    assert(flashremap_add(&t, "x", 0, 0, 0) == -1);
    assert(t.count == 0);
}

static void test_clear(void)
{
    FlashRemapTable t;
    flashremap_init(&t);
    flashremap_add(&t, "r1", 0x1000, 0x2000, 0x100);
    flashremap_add(&t, "r2", 0x3000, 0x4000, 0x200);
    assert(t.count == 2);

    flashremap_clear(&t);
    assert(t.count == 0);
    assert(flashremap_find(&t, "r1") == NULL);
}

int main(void)
{
    RUN_TEST(test_init);
    RUN_TEST(test_add_and_find);
    RUN_TEST(test_translate_hit);
    RUN_TEST(test_translate_miss);
    RUN_TEST(test_apply_region);
    RUN_TEST(test_add_invalid);
    RUN_TEST(test_clear);

    printf("flashremap: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
