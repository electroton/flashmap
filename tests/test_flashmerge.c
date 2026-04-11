#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashmerge.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_layout(void) {
    return flashlayout_create();
}

static void add_region(FlashLayout *layout, const char *name,
                       uint32_t start, uint32_t size) {
    FlashRegion r;
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';
    r.start = start;
    r.size  = size;
    flashlayout_add_region(layout, &r);
}

static void test_can_merge_adjacent(void) {
    FlashRegion a = { .start = 0x0000, .size = 0x1000 };
    FlashRegion b = { .start = 0x1000, .size = 0x1000 };
    assert(flashmerge_can_merge(&a, &b, MERGE_ADJACENT) == 1);
    assert(flashmerge_can_merge(&a, &b, MERGE_OVERLAPPING) == 0);
    assert(flashmerge_can_merge(&a, &b, MERGE_BOTH) == 1);
    printf("PASS: test_can_merge_adjacent\n");
}

static void test_can_merge_overlapping(void) {
    FlashRegion a = { .start = 0x0000, .size = 0x2000 };
    FlashRegion b = { .start = 0x1000, .size = 0x2000 };
    assert(flashmerge_can_merge(&a, &b, MERGE_OVERLAPPING) == 1);
    assert(flashmerge_can_merge(&a, &b, MERGE_ADJACENT)   == 0);
    assert(flashmerge_can_merge(&a, &b, MERGE_BOTH)       == 1);
    printf("PASS: test_can_merge_overlapping\n");
}

static void test_cannot_merge_gap(void) {
    FlashRegion a = { .start = 0x0000, .size = 0x1000 };
    FlashRegion b = { .start = 0x2000, .size = 0x1000 };
    assert(flashmerge_can_merge(&a, &b, MERGE_ADJACENT)   == 0);
    assert(flashmerge_can_merge(&a, &b, MERGE_OVERLAPPING) == 0);
    assert(flashmerge_can_merge(&a, &b, MERGE_BOTH)        == 0);
    printf("PASS: test_cannot_merge_gap\n");
}

static void test_combine_regions(void) {
    FlashRegion a = { .start = 0x0000, .size = 0x1000 };
    FlashRegion b = { .start = 0x1000, .size = 0x2000 };
    strncpy(a.name, "regionA", sizeof(a.name) - 1);
    strncpy(b.name, "regionB", sizeof(b.name) - 1);
    FlashRegion out;
    flashmerge_combine(&a, &b, &out);
    assert(out.start == 0x0000);
    assert(out.size  == 0x3000);
    assert(strcmp(out.name, "regionA") == 0);
    printf("PASS: test_combine_regions\n");
}

static void test_merge_apply_adjacent(void) {
    FlashLayout *layout = make_layout();
    add_region(layout, "boot",   0x0000, 0x1000);
    add_region(layout, "app",    0x1000, 0x2000);
    add_region(layout, "config", 0x5000, 0x1000); /* gap before this */

    MergeResult result = flashmerge_apply(layout, MERGE_ADJACENT);
    assert(result.layout != NULL);
    assert(result.merged_count == 1);
    assert(flashlayout_region_count(result.layout) == 2);

    flashlayout_destroy(result.layout);
    flashlayout_destroy(layout);
    printf("PASS: test_merge_apply_adjacent\n");
}

static void test_merge_apply_no_merge(void) {
    FlashLayout *layout = make_layout();
    add_region(layout, "a", 0x0000, 0x1000);
    add_region(layout, "b", 0x3000, 0x1000);

    MergeResult result = flashmerge_apply(layout, MERGE_BOTH);
    assert(result.layout != NULL);
    assert(result.merged_count == 0);
    assert(flashlayout_region_count(result.layout) == 2);

    flashlayout_destroy(result.layout);
    flashlayout_destroy(layout);
    printf("PASS: test_merge_apply_no_merge\n");
}

int main(void) {
    test_can_merge_adjacent();
    test_can_merge_overlapping();
    test_cannot_merge_gap();
    test_combine_regions();
    test_merge_apply_adjacent();
    test_merge_apply_no_merge();
    printf("All flashmerge tests passed.\n");
    return 0;
}
