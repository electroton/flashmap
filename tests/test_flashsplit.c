/**
 * test_flashsplit.c - Unit tests for flashsplit module
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashsplit.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size)
{
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    r.end   = start + size - 1;
    r.type  = FLASH_REGION_CODE;
    return r;
}

static void test_split_by_size_even(void)
{
    FlashRegion r = make_region("flash", 0x08000000, 0x1000);
    FlashSplitResult res = flashsplit_by_size(&r, 0x400);

    assert(res.count == 4);
    assert(res.regions[0].start == 0x08000000);
    assert(res.regions[0].size  == 0x400);
    assert(res.regions[3].start == 0x08000C00);
    assert(res.regions[3].end   == 0x08000FFF);

    flashsplit_result_free(&res);
    assert(res.regions == NULL);
    printf("PASS: test_split_by_size_even\n");
}

static void test_split_by_size_uneven(void)
{
    FlashRegion r = make_region("rom", 0x00000000, 0x1100);
    FlashSplitResult res = flashsplit_by_size(&r, 0x400);

    assert(res.count == 5);
    assert(res.regions[4].size == 0x100);
    assert(res.regions[4].start == 0x00001000);

    flashsplit_result_free(&res);
    printf("PASS: test_split_by_size_uneven\n");
}

static void test_split_by_size_invalid(void)
{
    FlashRegion r = make_region("x", 0x0, 0x100);
    FlashSplitResult res;

    res = flashsplit_by_size(NULL, 0x40);
    assert(res.regions == NULL && res.count == 0);

    res = flashsplit_by_size(&r, 0);
    assert(res.regions == NULL && res.count == 0);

    printf("PASS: test_split_by_size_invalid\n");
}

static void test_split_at_address_valid(void)
{
    FlashRegion r = make_region("data", 0x20000000, 0x2000);
    FlashRegion lo, hi;

    int rc = flashsplit_at_address(&r, 0x20001000, &lo, &hi);
    assert(rc == 0);
    assert(lo.start == 0x20000000);
    assert(lo.end   == 0x20000FFF);
    assert(lo.size  == 0x1000);
    assert(hi.start == 0x20001000);
    assert(hi.end   == 0x20001FFF);
    assert(hi.size  == 0x1000);

    printf("PASS: test_split_at_address_valid\n");
}

static void test_split_at_address_out_of_range(void)
{
    FlashRegion r = make_region("data", 0x20000000, 0x2000);
    FlashRegion lo, hi;

    /* Address before start */
    assert(flashsplit_at_address(&r, 0x1FFFFFFF, &lo, &hi) == -1);
    /* Address equal to start */
    assert(flashsplit_at_address(&r, 0x20000000, &lo, &hi) == -1);
    /* Address beyond end */
    assert(flashsplit_at_address(&r, 0x20002001, &lo, &hi) == -1);

    printf("PASS: test_split_at_address_out_of_range\n");
}

int main(void)
{
    test_split_by_size_even();
    test_split_by_size_uneven();
    test_split_by_size_invalid();
    test_split_at_address_valid();
    test_split_at_address_out_of_range();
    printf("All flashsplit tests passed.\n");
    return 0;
}
