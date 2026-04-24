#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashgroup.h"
#include "../src/flashgroup_report.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    return r;
}

static void test_create_destroy(void) {
    FlashGroup *g = flashgroup_create("test_group");
    assert(g != NULL);
    assert(strcmp(g->name, "test_group") == 0);
    assert(g->count == 0);
    flashgroup_destroy(g);
    printf("PASS test_create_destroy\n");
}

static void test_add_and_count(void) {
    FlashRegion r1 = make_region("bootloader", 0x00000000, 0x4000);
    FlashRegion r2 = make_region("application", 0x00004000, 0x1C000);

    FlashGroup *g = flashgroup_create("firmware");
    assert(flashgroup_add(g, &r1) == 0);
    assert(flashgroup_add(g, &r2) == 0);
    assert(g->count == 2);
    flashgroup_destroy(g);
    printf("PASS test_add_and_count\n");
}

static void test_total_size(void) {
    FlashRegion r1 = make_region("a", 0x0000, 0x1000);
    FlashRegion r2 = make_region("b", 0x1000, 0x2000);
    FlashRegion r3 = make_region("c", 0x3000, 0x0800);

    FlashGroup *g = flashgroup_create("sized");
    flashgroup_add(g, &r1);
    flashgroup_add(g, &r2);
    flashgroup_add(g, &r3);
    assert(flashgroup_total_size(g) == 0x3800);
    flashgroup_destroy(g);
    printf("PASS test_total_size\n");
}

static void test_find(void) {
    FlashRegion r1 = make_region("alpha", 0x0000, 0x100);
    FlashRegion r2 = make_region("beta",  0x0100, 0x200);

    FlashGroup *g = flashgroup_create("lookup");
    flashgroup_add(g, &r1);
    flashgroup_add(g, &r2);

    FlashRegion *found = flashgroup_find(g, "beta");
    assert(found != NULL);
    assert(found->start == 0x0100);

    assert(flashgroup_find(g, "gamma") == NULL);
    flashgroup_destroy(g);
    printf("PASS test_find\n");
}

static void test_remove(void) {
    FlashRegion r1 = make_region("x", 0x0000, 0x400);
    FlashRegion r2 = make_region("y", 0x0400, 0x400);
    FlashRegion r3 = make_region("z", 0x0800, 0x400);

    FlashGroup *g = flashgroup_create("removable");
    flashgroup_add(g, &r1);
    flashgroup_add(g, &r2);
    flashgroup_add(g, &r3);

    assert(flashgroup_remove(g, "y") == 0);
    assert(g->count == 2);
    assert(flashgroup_find(g, "y") == NULL);
    assert(flashgroup_remove(g, "missing") == -1);
    flashgroup_destroy(g);
    printf("PASS test_remove\n");
}

static void test_report(void) {
    FlashRegion r1 = make_region("boot", 0x00000000, 0x8000);
    FlashRegion r2 = make_region("app",  0x00008000, 0x18000);

    FlashGroup *g = flashgroup_create("full_flash");
    flashgroup_add(g, &r1);
    flashgroup_add(g, &r2);

    flashgroup_report_summary(g, stdout);
    flashgroup_report_detail(g, stdout);

    flashgroup_destroy(g);
    printf("PASS test_report\n");
}

int main(void) {
    test_create_destroy();
    test_add_and_count();
    test_total_size();
    test_find();
    test_remove();
    test_report();
    printf("All flashgroup tests passed.\n");
    return 0;
}
