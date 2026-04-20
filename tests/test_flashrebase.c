#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/flashrebase.h"
#include "../src/flashregion.h"
#include "../src/flashlayout.h"

/* ------------------------------------------------------------------ helpers */

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size)
{
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    return r;
}

static FlashLayout make_layout_two(void)
{
    FlashLayout l;
    flash_layout_init(&l);
    FlashRegion a = make_region("text",  0x08000000, 0x10000);
    FlashRegion b = make_region("data",  0x08010000, 0x4000);
    flash_layout_add(&l, &a);
    flash_layout_add(&l, &b);
    return l;
}

/* ------------------------------------------------------------------- tests */

static void test_rebase_region_positive_offset(void)
{
    FlashRegion r = make_region("code", 0x08000000, 0x8000);
    FlashRebaseResult res = flash_rebase_region(&r, 0x10000);
    assert(res == REBASE_OK);
    assert(r.start == 0x08010000);
    printf("PASS: test_rebase_region_positive_offset\n");
}

static void test_rebase_region_negative_offset(void)
{
    FlashRegion r = make_region("code", 0x08010000, 0x8000);
    FlashRebaseResult res = flash_rebase_region(&r, -0x10000);
    assert(res == REBASE_OK);
    assert(r.start == 0x08000000);
    printf("PASS: test_rebase_region_negative_offset\n");
}

static void test_rebase_region_underflow(void)
{
    FlashRegion r = make_region("code", 0x00000100, 0x100);
    /* Shifting below 0 should fail */
    FlashRebaseResult res = flash_rebase_region(&r, -0x200);
    assert(res == REBASE_ERR_UNDERFLOW);
    /* Region must remain unchanged */
    assert(r.start == 0x00000100);
    printf("PASS: test_rebase_region_underflow\n");
}

static void test_rebase_region_null(void)
{
    FlashRebaseResult res = flash_rebase_region(NULL, 0x1000);
    assert(res == REBASE_ERR_NULL);
    printf("PASS: test_rebase_region_null\n");
}

static void test_rebase_layout_uniform_offset(void)
{
    FlashLayout l = make_layout_two();
    FlashRebaseResult res = flash_rebase_layout(&l, 0x20000);
    assert(res == REBASE_OK);
    assert(l.regions[0].start == 0x08020000);
    assert(l.regions[1].start == 0x08030000);
    flash_layout_free(&l);
    printf("PASS: test_rebase_layout_uniform_offset\n");
}

static void test_rebase_layout_to_new_base(void)
{
    FlashLayout l = make_layout_two();
    /* Rebase so the lowest region starts at 0x20000000 */
    FlashRebaseResult res = flash_rebase_layout_to(&l, 0x20000000);
    assert(res == REBASE_OK);
    assert(l.regions[0].start == 0x20000000);
    assert(l.regions[1].start == 0x20010000);
    flash_layout_free(&l);
    printf("PASS: test_rebase_layout_to_new_base\n");
}

static void test_rebase_min_address(void)
{
    FlashLayout l = make_layout_two();
    uint32_t min = 0;
    FlashRebaseResult res = flash_rebase_min_address(&l, &min);
    assert(res == REBASE_OK);
    assert(min == 0x08000000);
    flash_layout_free(&l);
    printf("PASS: test_rebase_min_address\n");
}

/* -------------------------------------------------------------------  main */

int main(void)
{
    test_rebase_region_positive_offset();
    test_rebase_region_negative_offset();
    test_rebase_region_underflow();
    test_rebase_region_null();
    test_rebase_layout_uniform_offset();
    test_rebase_layout_to_new_base();
    test_rebase_min_address();
    printf("All flashrebase tests passed.\n");
    return 0;
}
