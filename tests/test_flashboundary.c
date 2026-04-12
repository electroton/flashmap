/*
 * test_flashboundary.c - Unit tests for flashboundary module
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "../src/flashboundary.h"
#include "../src/flashregion.h"

static FlashRegion make_region(uint32_t addr, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    r.address = addr;
    r.size    = size;
    return r;
}

static void test_boundary_check_ok(void) {
    FlashRegion r = make_region(0x08000000, 0x10000);
    assert(flash_boundary_check(&r, 0x08000000, 0x10000) == FLASH_BOUNDARY_OK);
    assert(flash_boundary_check(&r, 0x08001000, 0x100)   == FLASH_BOUNDARY_OK);
    printf("PASS: test_boundary_check_ok\n");
}

static void test_boundary_check_overflow(void) {
    FlashRegion r = make_region(0x08000000, 0x1000);
    FlashBoundaryResult res = flash_boundary_check(&r, 0x08000F00, 0x200);
    assert(res == FLASH_BOUNDARY_OVERFLOW);
    printf("PASS: test_boundary_check_overflow\n");
}

static void test_boundary_check_underflow(void) {
    FlashRegion r = make_region(0x08000100, 0x1000);
    FlashBoundaryResult res = flash_boundary_check(&r, 0x08000000, 0x200);
    assert(res == FLASH_BOUNDARY_UNDERFLOW);
    printf("PASS: test_boundary_check_underflow\n");
}

static void test_boundary_check_zero_size(void) {
    FlashRegion r = make_region(0x08000000, 0x1000);
    assert(flash_boundary_check(&r, 0x08000100, 0) == FLASH_BOUNDARY_ZERO_SIZE);
    printf("PASS: test_boundary_check_zero_size\n");
}

static void test_boundary_check_null(void) {
    assert(flash_boundary_check(NULL, 0, 4) == FLASH_BOUNDARY_INVALID_ARG);
    printf("PASS: test_boundary_check_null\n");
}

static void test_boundary_contains(void) {
    FlashRegion r = make_region(0x20000000, 0x8000);
    assert(flash_boundary_contains(&r, 0x20000000) == true);
    assert(flash_boundary_contains(&r, 0x20004000) == true);
    assert(flash_boundary_contains(&r, 0x20008000) == false); /* past end */
    assert(flash_boundary_contains(&r, 0x1FFFFFFF) == false);
    assert(flash_boundary_contains(NULL, 0x20000000) == false);
    printf("PASS: test_boundary_contains\n");
}

static void test_boundary_overlaps(void) {
    FlashRegion a = make_region(0x1000, 0x1000); /* 0x1000..0x1FFF */
    FlashRegion b = make_region(0x1800, 0x1000); /* 0x1800..0x27FF */
    FlashRegion c = make_region(0x2000, 0x1000); /* 0x2000..0x2FFF */

    assert(flash_boundary_overlaps(&a, &b) == true);
    assert(flash_boundary_overlaps(&b, &c) == true);
    assert(flash_boundary_overlaps(&a, &c) == false); /* adjacent, not overlapping */
    printf("PASS: test_boundary_overlaps\n");
}

static void test_boundary_find_violations(void) {
    FlashRegion regions[3];
    regions[0] = make_region(0x0000, 0x1000);
    regions[1] = make_region(0x0800, 0x1000); /* overlaps [0] */
    regions[2] = make_region(0x4000, 0x2000); /* no overlap */

    size_t count = 0;
    FlashBoundaryViolation *v = flash_boundary_find_violations(regions, 3, &count);
    assert(count == 1);
    assert(v != NULL);
    assert(v[0].region_a_index == 0);
    assert(v[0].region_b_index == 1);
    assert(v[0].overlap_start  == 0x0800);
    assert(v[0].overlap_end    == 0x1000);
    flash_boundary_free_violations(v);
    printf("PASS: test_boundary_find_violations\n");
}

static void test_boundary_no_violations(void) {
    FlashRegion regions[2];
    regions[0] = make_region(0x0000, 0x1000);
    regions[1] = make_region(0x1000, 0x1000);

    size_t count = 99;
    FlashBoundaryViolation *v = flash_boundary_find_violations(regions, 2, &count);
    assert(count == 0);
    flash_boundary_free_violations(v);
    printf("PASS: test_boundary_no_violations\n");
}

static void test_boundary_result_str(void) {
    assert(strcmp(flash_boundary_result_str(FLASH_BOUNDARY_OK),       "OK")       == 0);
    assert(strcmp(flash_boundary_result_str(FLASH_BOUNDARY_OVERFLOW),  "overflow") == 0);
    assert(strcmp(flash_boundary_result_str(FLASH_BOUNDARY_UNDERFLOW), "underflow") == 0);
    printf("PASS: test_boundary_result_str\n");
}

int main(void) {
    test_boundary_check_ok();
    test_boundary_check_overflow();
    test_boundary_check_underflow();
    test_boundary_check_zero_size();
    test_boundary_check_null();
    test_boundary_contains();
    test_boundary_overlaps();
    test_boundary_find_violations();
    test_boundary_no_violations();
    test_boundary_result_str();
    printf("All flashboundary tests passed.\n");
    return 0;
}
