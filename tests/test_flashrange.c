#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashrange.h"

static void test_from_size(void)
{
    FlashRange r = flashrange_from_size(0x1000, 0x200, "boot");
    assert(r.start == 0x1000);
    assert(r.end   == 0x11FF);
    assert(strcmp(r.name, "boot") == 0);
    assert(flashrange_size(&r) == 0x200);
    printf("PASS test_from_size\n");
}

static void test_from_size_zero(void)
{
    FlashRange r = flashrange_from_size(0x2000, 0, "empty");
    assert(r.start == r.end);
    assert(flashrange_size(&r) == 1); /* end == start => 1 byte */
    printf("PASS test_from_size_zero\n");
}

static void test_contains(void)
{
    FlashRange r = flashrange_from_size(0x1000, 0x100, "r");
    assert( flashrange_contains(&r, 0x1000));
    assert( flashrange_contains(&r, 0x1050));
    assert( flashrange_contains(&r, 0x10FF));
    assert(!flashrange_contains(&r, 0x0FFF));
    assert(!flashrange_contains(&r, 0x1100));
    printf("PASS test_contains\n");
}

static void test_overlaps(void)
{
    FlashRange a = flashrange_from_size(0x1000, 0x100, "a");
    FlashRange b = flashrange_from_size(0x10C0, 0x100, "b");
    FlashRange c = flashrange_from_size(0x2000, 0x100, "c");
    assert( flashrange_overlaps(&a, &b));
    assert( flashrange_overlaps(&b, &a));
    assert(!flashrange_overlaps(&a, &c));
    assert(!flashrange_overlaps(&c, &a));
    printf("PASS test_overlaps\n");
}

static void test_intersect(void)
{
    FlashRange a = flashrange_from_size(0x1000, 0x100, "a");
    FlashRange b = flashrange_from_size(0x10C0, 0x100, "b");
    FlashRangeResult res = flashrange_intersect(&a, &b);
    assert(res.valid);
    assert(res.range.start == 0x10C0);
    assert(res.range.end   == 0x10FF);
    assert(flashrange_size(&res.range) == 0x40);
    printf("PASS test_intersect\n");
}

static void test_intersect_disjoint(void)
{
    FlashRange a = flashrange_from_size(0x1000, 0x100, "a");
    FlashRange b = flashrange_from_size(0x2000, 0x100, "b");
    FlashRangeResult res = flashrange_intersect(&a, &b);
    assert(!res.valid);
    printf("PASS test_intersect_disjoint\n");
}

static void test_clip(void)
{
    FlashRange r    = flashrange_from_size(0x0F00, 0x300, "r");
    FlashRange bound = flashrange_from_size(0x1000, 0x1000, "flash");
    FlashRangeResult res = flashrange_clip(&r, &bound);
    assert(res.valid);
    assert(res.range.start == 0x1000);
    assert(res.range.end   == 0x11FF);
    printf("PASS test_clip\n");
}

static void test_null_safety(void)
{
    assert(!flashrange_contains(NULL, 0x1000));
    assert(!flashrange_overlaps(NULL, NULL));
    FlashRangeResult res = flashrange_intersect(NULL, NULL);
    assert(!res.valid);
    assert(flashrange_size(NULL) == 0);
    printf("PASS test_null_safety\n");
}

int main(void)
{
    test_from_size();
    test_from_size_zero();
    test_contains();
    test_overlaps();
    test_intersect();
    test_intersect_disjoint();
    test_clip();
    test_null_safety();
    printf("All flashrange tests passed.\n");
    return 0;
}
