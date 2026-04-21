#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashclamp.h"

static FlashRegion make_region(const char *name, uint32_t addr, uint32_t size)
{
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.address = addr;
    r.size    = size;
    return r;
}

static void test_clamp_fully_inside(void)
{
    FlashRegion reg = make_region("code", 0x1000, 0x200);
    FlashClampBounds bounds = { .start = 0x0000, .end = 0x3FFF };
    FlashRegion out;

    FlashClampResult r = flashclamp_region(&reg, &bounds, &out);
    assert(r == FLASHCLAMP_OK);
    assert(out.address == 0x1000);
    assert(out.size    == 0x200);
    printf("PASS: test_clamp_fully_inside\n");
}

static void test_clamp_trim_start(void)
{
    FlashRegion reg = make_region("data", 0x0800, 0x400);
    FlashClampBounds bounds = { .start = 0x0A00, .end = 0x1FFF };
    FlashRegion out;

    FlashClampResult r = flashclamp_region(&reg, &bounds, &out);
    assert(r == FLASHCLAMP_OK);
    assert(out.address == 0x0A00);
    assert(out.size    == 0x0C00 - 0x0A00 + 1);  /* trimmed from start */
    printf("PASS: test_clamp_trim_start\n");
}

static void test_clamp_trim_end(void)
{
    FlashRegion reg = make_region("bss", 0x2000, 0x1000);
    FlashClampBounds bounds = { .start = 0x1000, .end = 0x2800 };
    FlashRegion out;

    FlashClampResult r = flashclamp_region(&reg, &bounds, &out);
    assert(r == FLASHCLAMP_OK);
    assert(out.address == 0x2000);
    assert(out.size    == 0x2800 - 0x2000 + 1);
    printf("PASS: test_clamp_trim_end\n");
}

static void test_clamp_no_overlap(void)
{
    FlashRegion reg = make_region("heap", 0x5000, 0x200);
    FlashClampBounds bounds = { .start = 0x0000, .end = 0x0FFF };
    FlashRegion out;

    FlashClampResult r = flashclamp_region(&reg, &bounds, &out);
    assert(r == FLASHCLAMP_ERR_NO_OVERLAP);
    printf("PASS: test_clamp_no_overlap\n");
}

static void test_clamp_invalid_bounds(void)
{
    FlashRegion reg = make_region("stack", 0x1000, 0x100);
    FlashClampBounds bounds = { .start = 0x2000, .end = 0x1000 };
    FlashRegion out;

    FlashClampResult r = flashclamp_region(&reg, &bounds, &out);
    assert(r == FLASHCLAMP_ERR_INVALID_BOUNDS);
    printf("PASS: test_clamp_invalid_bounds\n");
}

static void test_clamp_regions_mixed(void)
{
    FlashRegion regs[4] = {
        make_region("r0", 0x0000, 0x0200),  /* before window */
        make_region("r1", 0x0800, 0x0400),  /* overlaps start */
        make_region("r2", 0x1000, 0x0200),  /* fully inside */
        make_region("r3", 0x1E00, 0x0400)   /* overlaps end */
    };
    FlashClampBounds bounds = { .start = 0x0A00, .end = 0x1FFF };
    FlashRegion out[4];
    size_t out_count = 0;

    FlashClampResult r = flashclamp_regions(regs, 4, &bounds, out, &out_count);
    assert(r == FLASHCLAMP_OK);
    assert(out_count == 3);  /* r0 excluded, r1/r2/r3 clamped */
    assert(out[0].address == 0x0A00);
    assert(out[1].address == 0x1000);
    assert(out[2].address == 0x1E00);
    assert(out[2].size    == 0x1FFF - 0x1E00 + 1);
    printf("PASS: test_clamp_regions_mixed\n");
}

static void test_clamp_null_args(void)
{
    FlashRegion reg = make_region("x", 0x100, 0x100);
    FlashClampBounds bounds = { .start = 0x0, .end = 0xFFFF };
    FlashRegion out;

    assert(flashclamp_region(NULL,  &bounds, &out) == FLASHCLAMP_ERR_NULL);
    assert(flashclamp_region(&reg,  NULL,    &out) == FLASHCLAMP_ERR_NULL);
    assert(flashclamp_region(&reg,  &bounds, NULL) == FLASHCLAMP_ERR_NULL);
    printf("PASS: test_clamp_null_args\n");
}

int main(void)
{
    test_clamp_fully_inside();
    test_clamp_trim_start();
    test_clamp_trim_end();
    test_clamp_no_overlap();
    test_clamp_invalid_bounds();
    test_clamp_regions_mixed();
    test_clamp_null_args();
    printf("All flashclamp tests passed.\n");
    return 0;
}
