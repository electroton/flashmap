#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/flashresize.h"

/* ---- helpers ---- */

static FlashRegion make_region(const char *name, uint32_t addr,
                               uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.address = addr;
    r.size    = size;
    r.used    = used;
    return r;
}

/* ---- tests ---- */

static void test_basic_grow(void) {
    FlashRegion r = make_region("text", 0x0000, 0x1000, 0x800);
    FlashResizeResult res = flash_resize_region(&r, 0x2000, NULL);
    assert(res == RESIZE_OK);
    assert(r.size == 0x2000);
    printf("PASS: test_basic_grow\n");
}

static void test_basic_shrink(void) {
    FlashRegion r = make_region("data", 0x2000, 0x1000, 0x400);
    FlashResizeResult res = flash_resize_region(&r, 0x800, NULL);
    assert(res == RESIZE_OK);
    assert(r.size == 0x800);
    printf("PASS: test_basic_shrink\n");
}

static void test_shrink_below_used(void) {
    FlashRegion r = make_region("bss", 0x3000, 0x1000, 0x900);
    FlashResizeResult res = flash_resize_region(&r, 0x500, NULL);
    assert(res == RESIZE_ERR_TOO_SMALL);
    printf("PASS: test_shrink_below_used\n");
}

static void test_overflow(void) {
    FlashRegion r = make_region("boot", 0xF000, 0x1000, 0x100);
    FlashResizeOptions opts = {true, false, 1, 0xFFFF};
    /* 0xF000 + 0x2000 = 0x11000 > 0xFFFF */
    FlashResizeResult res = flash_resize_region(&r, 0x2000, &opts);
    assert(res == RESIZE_ERR_OVERFLOW);
    printf("PASS: test_overflow\n");
}

static void test_align_up(void) {
    FlashRegion r = make_region("cfg", 0x0000, 0x100, 0x10);
    FlashResizeOptions opts = {true, true, 256, 0xFFFFFFFF};
    FlashResizeResult res = flash_resize_region(&r, 0x110, &opts);
    assert(res == RESIZE_OK);
    assert(r.size == 0x200); /* aligned up to 256 */
    printf("PASS: test_align_up\n");
}

static void test_invalid_alignment(void) {
    FlashRegion r = make_region("cfg", 0x0000, 0x100, 0x10);
    FlashResizeOptions opts = {true, true, 300, 0xFFFFFFFF}; /* not power of 2 */
    FlashResizeResult res = flash_resize_region(&r, 0x200, &opts);
    assert(res == RESIZE_ERR_ALIGN);
    printf("PASS: test_invalid_alignment\n");
}

static void test_named_resize_ok(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions[0] = make_region("text",  0x0000, 0x1000, 0x500);
    layout.regions[1] = make_region("data",  0x2000, 0x1000, 0x200);
    layout.region_count = 2;

    FlashResizeResult res = flash_resize_named(&layout, "text", 0x1800, NULL);
    assert(res == RESIZE_OK);
    assert(layout.regions[0].size == 0x1800);
    printf("PASS: test_named_resize_ok\n");
}

static void test_named_not_found(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions[0] = make_region("text", 0x0000, 0x1000, 0x500);
    layout.region_count = 1;

    FlashResizeResult res = flash_resize_named(&layout, "missing", 0x2000, NULL);
    assert(res == RESIZE_ERR_NOT_FOUND);
    printf("PASS: test_named_not_found\n");
}

static void test_named_overlap(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions[0] = make_region("text", 0x0000, 0x1000, 0x100);
    layout.regions[1] = make_region("data", 0x1000, 0x1000, 0x200);
    layout.region_count = 2;

    /* Growing text into data's space */
    FlashResizeResult res = flash_resize_named(&layout, "text", 0x1800, NULL);
    assert(res == RESIZE_ERR_OVERLAP);
    printf("PASS: test_named_overlap\n");
}

static void test_result_str(void) {
    assert(strcmp(flash_resize_result_str(RESIZE_OK), "OK") == 0);
    assert(flash_resize_result_str(RESIZE_ERR_OVERFLOW) != NULL);
    printf("PASS: test_result_str\n");
}

int main(void) {
    test_basic_grow();
    test_basic_shrink();
    test_shrink_below_used();
    test_overflow();
    test_align_up();
    test_invalid_alignment();
    test_named_resize_ok();
    test_named_not_found();
    test_named_overlap();
    test_result_str();
    printf("All flashresize tests passed.\n");
    return 0;
}
