#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashoffset.h"
#include "../src/flashregion.h"
#include "../src/flashlayout.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';
    r.start = start;
    r.size  = size;
    return r;
}

static void test_offset_add(void) {
    FlashRegion r = make_region("text", 0x08000000, 0x10000);
    FlashOffsetResult res = flash_offset_apply(&r, 0x1000, FLASH_OFFSET_ADD);
    assert(res.status    == FLASH_OFFSET_OK);
    assert(res.new_start == 0x08001000);
    assert(res.new_end   == 0x08011000);
    assert(res.delta     == 0x1000);
    printf("PASS: test_offset_add\n");
}

static void test_offset_sub(void) {
    FlashRegion r = make_region("data", 0x08010000, 0x4000);
    FlashOffsetResult res = flash_offset_apply(&r, 0x1000, FLASH_OFFSET_SUB);
    assert(res.status    == FLASH_OFFSET_OK);
    assert(res.new_start == 0x0800F000);
    assert(res.new_end   == 0x08013000);
    assert(res.delta     == -0x1000);
    printf("PASS: test_offset_sub\n");
}

static void test_offset_sub_underflow(void) {
    FlashRegion r = make_region("bss", 0x0100, 0x200);
    FlashOffsetResult res = flash_offset_apply(&r, 0x0200, FLASH_OFFSET_SUB);
    assert(res.status == FLASH_OFFSET_ERR_UNDERFLOW);
    printf("PASS: test_offset_sub_underflow\n");
}

static void test_offset_abs(void) {
    FlashRegion r = make_region("boot", 0x08000000, 0x2000);
    FlashOffsetResult res = flash_offset_apply(&r, 0x20000000, FLASH_OFFSET_ABS);
    assert(res.status    == FLASH_OFFSET_OK);
    assert(res.new_start == 0x20000000);
    assert(res.new_end   == 0x20002000);
    printf("PASS: test_offset_abs\n");
}

static void test_null_region(void) {
    FlashOffsetResult res = flash_offset_apply(NULL, 0x100, FLASH_OFFSET_ADD);
    assert(res.status == FLASH_OFFSET_ERR_NULL);
    printf("PASS: test_null_region\n");
}

static void test_overlap_detection(void) {
    FlashRegion a = make_region("a", 0x1000, 0x2000);
    FlashRegion b = make_region("b", 0x2000, 0x2000);
    FlashOffsetResult ra = flash_offset_apply(&a, 0x1000, FLASH_OFFSET_ADD);
    FlashOffsetResult rb = flash_offset_apply(&b, 0,      FLASH_OFFSET_ADD);
    assert(ra.status == FLASH_OFFSET_OK);
    assert(rb.status == FLASH_OFFSET_OK);
    /* ra: 0x2000..0x4000, rb: 0x2000..0x4000 — should overlap */
    assert(flash_offset_would_overlap(&ra, &rb) == true);
    printf("PASS: test_overlap_detection\n");
}

static void test_status_strings(void) {
    assert(strcmp(flash_offset_status_str(FLASH_OFFSET_OK),            "OK") == 0);
    assert(strcmp(flash_offset_status_str(FLASH_OFFSET_ERR_UNDERFLOW), "Underflow: delta exceeds region start") == 0);
    printf("PASS: test_status_strings\n");
}

int main(void) {
    test_offset_add();
    test_offset_sub();
    test_offset_sub_underflow();
    test_offset_abs();
    test_null_region();
    test_overlap_detection();
    test_status_strings();
    printf("All flashoffset tests passed.\n");
    return 0;
}
