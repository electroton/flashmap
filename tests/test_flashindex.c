#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashindex.h"

/* Minimal FlashRegion definition for testing */
static FlashRegion make_region(const char *name, uint32_t base, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.base = base;
    r.size = size;
    return r;
}

static void test_init_and_clear(void) {
    FlashIndex idx;
    flash_index_init(&idx);
    assert(idx.count == 0);
    assert(idx.sorted == 0);

    FlashRegion r = make_region("boot", 0x0000, 0x1000);
    flash_index_add(&idx, &r);
    assert(idx.count == 1);

    flash_index_clear(&idx);
    assert(idx.count == 0);
    printf("PASS: test_init_and_clear\n");
}

static void test_add_and_sort(void) {
    FlashIndex idx;
    flash_index_init(&idx);

    FlashRegion r1 = make_region("app",  0x4000, 0x8000);
    FlashRegion r2 = make_region("boot", 0x0000, 0x4000);
    FlashRegion r3 = make_region("data", 0xC000, 0x4000);

    assert(flash_index_add(&idx, &r1) == 0);
    assert(flash_index_add(&idx, &r2) == 0);
    assert(flash_index_add(&idx, &r3) == 0);
    assert(idx.count == 3);

    flash_index_sort(&idx);
    assert(idx.sorted == 1);
    assert(idx.entries[0]->base == 0x0000);
    assert(idx.entries[1]->base == 0x4000);
    assert(idx.entries[2]->base == 0xC000);
    printf("PASS: test_add_and_sort\n");
}

static void test_find_hit_and_miss(void) {
    FlashIndex idx;
    flash_index_init(&idx);

    FlashRegion r1 = make_region("boot", 0x0000, 0x2000);
    FlashRegion r2 = make_region("app",  0x2000, 0x6000);

    flash_index_add(&idx, &r1);
    flash_index_add(&idx, &r2);
    flash_index_sort(&idx);

    const FlashRegion *found = flash_index_find(&idx, 0x0000);
    assert(found != NULL);
    assert(found->base == 0x0000);

    found = flash_index_find(&idx, 0x1FFF);
    assert(found != NULL && found->base == 0x0000);

    found = flash_index_find(&idx, 0x2000);
    assert(found != NULL && found->base == 0x2000);

    found = flash_index_find(&idx, 0x7FFF);
    assert(found != NULL && found->base == 0x2000);

    found = flash_index_find(&idx, 0x8000);
    assert(found == NULL);

    printf("PASS: test_find_hit_and_miss\n");
}

static void test_find_range(void) {
    FlashIndex idx;
    flash_index_init(&idx);

    FlashRegion r1 = make_region("a", 0x0000, 0x1000);
    FlashRegion r2 = make_region("b", 0x1000, 0x1000);
    FlashRegion r3 = make_region("c", 0x3000, 0x1000);

    flash_index_add(&idx, &r1);
    flash_index_add(&idx, &r2);
    flash_index_add(&idx, &r3);
    flash_index_sort(&idx);

    const FlashRegion *results[8];
    size_t n = flash_index_find_range(&idx, 0x0800, 0x1000, results, 8);
    assert(n == 2); /* overlaps r1 and r2 */

    n = flash_index_find_range(&idx, 0x2000, 0x0800, results, 8);
    assert(n == 0); /* gap between r2 and r3 */

    n = flash_index_find_range(&idx, 0x0000, 0x4000, results, 8);
    assert(n == 3);

    printf("PASS: test_find_range\n");
}

static void test_find_unsorted_returns_null(void) {
    FlashIndex idx;
    flash_index_init(&idx);
    FlashRegion r = make_region("x", 0x0000, 0x1000);
    flash_index_add(&idx, &r);
    /* Do NOT sort */
    const FlashRegion *found = flash_index_find(&idx, 0x0000);
    assert(found == NULL);
    printf("PASS: test_find_unsorted_returns_null\n");
}

int main(void) {
    test_init_and_clear();
    test_add_and_sort();
    test_find_hit_and_miss();
    test_find_range();
    test_find_unsorted_returns_null();
    printf("All flashindex tests passed.\n");
    return 0;
}
