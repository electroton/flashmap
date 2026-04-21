#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../src/flashoffset.h"
#include "../src/flashregion.h"

/* ---- helpers ---- */
static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    r.name  = name;
    r.start = start;
    r.size  = size;
    return r;
}

/* ---- flashoffset_compute ---- */
static void test_compute_basic(void) {
    FlashRegion r = make_region("text", 0x08002000, 0x4000);
    FlashOffsetEntry e;
    FlashOffsetResult res = flashoffset_compute(&r, 0x08000000, &e);
    assert(res == FLASHOFFSET_OK);
    assert(e.offset == 0x2000);
    assert(e.size   == 0x4000);
    assert(e.base   == 0x08000000);
    assert(strcmp(e.name, "text") == 0);
    printf("PASS: test_compute_basic\n");
}

static void test_compute_zero_base(void) {
    FlashRegion r = make_region("data", 0x0001000, 0x800);
    FlashOffsetEntry e;
    FlashOffsetResult res = flashoffset_compute(&r, 0x0, &e);
    assert(res == FLASHOFFSET_OK);
    assert(e.offset == 0x1000);
    printf("PASS: test_compute_zero_base\n");
}

static void test_compute_underflow(void) {
    FlashRegion r = make_region("bss", 0x1000, 0x200);
    FlashOffsetEntry e;
    FlashOffsetResult res = flashoffset_compute(&r, 0x2000, &e);
    assert(res == FLASHOFFSET_ERR_UNDERFLOW);
    printf("PASS: test_compute_underflow\n");
}

static void test_compute_null(void) {
    FlashOffsetEntry e;
    assert(flashoffset_compute(NULL, 0, &e) == FLASHOFFSET_ERR_NULL);
    FlashRegion r = make_region("x", 0, 0);
    assert(flashoffset_compute(&r, 0, NULL) == FLASHOFFSET_ERR_NULL);
    printf("PASS: test_compute_null\n");
}

/* ---- flashoffset_shift ---- */
static void test_shift_positive(void) {
    FlashRegion regions[2] = {
        make_region("a", 0x1000, 0x200),
        make_region("b", 0x1200, 0x400)
    };
    assert(flashoffset_shift(regions, 2, 0x100) == FLASHOFFSET_OK);
    assert(regions[0].start == 0x1100);
    assert(regions[1].start == 0x1300);
    printf("PASS: test_shift_positive\n");
}

static void test_shift_negative(void) {
    FlashRegion regions[1] = { make_region("c", 0x2000, 0x100) };
    assert(flashoffset_shift(regions, 1, -0x1000) == FLASHOFFSET_OK);
    assert(regions[0].start == 0x1000);
    printf("PASS: test_shift_negative\n");
}

static void test_shift_underflow(void) {
    FlashRegion regions[1] = { make_region("d", 0x0100, 0x100) };
    assert(flashoffset_shift(regions, 1, -0x200) == FLASHOFFSET_ERR_UNDERFLOW);
    printf("PASS: test_shift_underflow\n");
}

/* ---- flashoffset_normalize ---- */
static void test_normalize(void) {
    FlashRegion regions[3] = {
        make_region("x", 0x08004000, 0x1000),
        make_region("y", 0x08000000, 0x2000),
        make_region("z", 0x08006000, 0x800)
    };
    assert(flashoffset_normalize(regions, 3) == FLASHOFFSET_OK);
    /* minimum was 0x08000000; all should be shifted down by that */
    assert(regions[1].start == 0x0000);
    assert(regions[0].start == 0x4000);
    assert(regions[2].start == 0x6000);
    printf("PASS: test_normalize\n");
}

static void test_normalize_null(void) {
    assert(flashoffset_normalize(NULL, 3) == FLASHOFFSET_ERR_NULL);
    printf("PASS: test_normalize_null\n");
}

int main(void) {
    test_compute_basic();
    test_compute_zero_base();
    test_compute_underflow();
    test_compute_null();
    test_shift_positive();
    test_shift_negative();
    test_shift_underflow();
    test_normalize();
    test_normalize_null();
    printf("All flashoffset tests passed.\n");
    return 0;
}
