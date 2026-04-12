#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashboundary.h"
#include "../src/flashregion.h"

static void make_region(FlashRegion *r, const char *name, uint32_t addr, uint32_t size) {
    strncpy(r->name, name, sizeof(r->name) - 1);
    r->name[sizeof(r->name) - 1] = '\0';
    r->address = addr;
    r->size    = size;
}

static void test_init_and_add(void) {
    FlashBoundarySet set;
    flashboundary_init(&set);
    assert(set.count == 0);

    int rc = flashboundary_add(&set, 0x08000000, 0x08100000, 0x200, "FLASH");
    assert(rc == 0);
    assert(set.count == 1);
    assert(set.bounds[0].base == 0x08000000);
    assert(set.bounds[0].limit == 0x08100000);
    printf("PASS: test_init_and_add\n");
}

static void test_region_within_bounds(void) {
    FlashBoundarySet set;
    flashboundary_init(&set);
    flashboundary_add(&set, 0x08000000, 0x08100000, 0x200, "FLASH");

    FlashRegion r;
    make_region(&r, "code", 0x08000000, 0x1000);

    BoundaryViolation v;
    BoundaryStatus s = flashboundary_check(&set, &r, &v);
    assert(s == BOUNDARY_OK);
    printf("PASS: test_region_within_bounds\n");
}

static void test_region_overflow(void) {
    FlashBoundarySet set;
    flashboundary_init(&set);
    flashboundary_add(&set, 0x08000000, 0x08001000, 0, "SMALL");

    FlashRegion r;
    make_region(&r, "big", 0x08000800, 0x1000); /* ends at 0x08001800 > limit */

    BoundaryViolation v;
    BoundaryStatus s = flashboundary_check(&set, &r, &v);
    assert(s == BOUNDARY_OVERFLOW);
    assert(v.violation_addr == 0x08001800);
    printf("PASS: test_region_overflow\n");
}

static void test_region_underflow(void) {
    FlashBoundarySet set;
    flashboundary_init(&set);
    flashboundary_add(&set, 0x08001000, 0x08010000, 0, "UPPER");

    FlashRegion r;
    make_region(&r, "low", 0x08000800, 0x200); /* starts below base */

    BoundaryViolation v;
    BoundaryStatus s = flashboundary_check(&set, &r, &v);
    assert(s == BOUNDARY_UNDERFLOW);
    printf("PASS: test_region_underflow\n");
}

static void test_misaligned_region(void) {
    FlashBoundarySet set;
    flashboundary_init(&set);
    flashboundary_add(&set, 0x08000000, 0x08100000, 0x200, "FLASH");

    FlashRegion r;
    make_region(&r, "odd", 0x08000100, 0x200); /* not aligned to 0x200 */

    BoundaryViolation v;
    BoundaryStatus s = flashboundary_check(&set, &r, &v);
    assert(s == BOUNDARY_MISALIGNED);
    printf("PASS: test_misaligned_region\n");
}

static void test_check_all(void) {
    FlashBoundarySet set;
    flashboundary_init(&set);
    flashboundary_add(&set, 0x08000000, 0x08010000, 0, "FLASH");

    FlashRegion regions[3];
    make_region(&regions[0], "ok",       0x08000000, 0x1000);
    make_region(&regions[1], "overflow", 0x0800F000, 0x2000);
    make_region(&regions[2], "ok2",      0x08002000, 0x500);

    BoundaryViolation violations[3];
    int n = flashboundary_check_all(&set, regions, 3, violations, 3);
    assert(n == 1);
    assert(violations[0].status == BOUNDARY_OVERFLOW);
    printf("PASS: test_check_all\n");
}

static void test_status_str(void) {
    assert(strcmp(flashboundary_status_str(BOUNDARY_OK),         "OK")         == 0);
    assert(strcmp(flashboundary_status_str(BOUNDARY_OVERFLOW),   "OVERFLOW")   == 0);
    assert(strcmp(flashboundary_status_str(BOUNDARY_UNDERFLOW),  "UNDERFLOW")  == 0);
    assert(strcmp(flashboundary_status_str(BOUNDARY_MISALIGNED), "MISALIGNED") == 0);
    printf("PASS: test_status_str\n");
}

int main(void) {
    test_init_and_add();
    test_region_within_bounds();
    test_region_overflow();
    test_region_underflow();
    test_misaligned_region();
    test_check_all();
    test_status_str();
    printf("All flashboundary tests passed.\n");
    return 0;
}
