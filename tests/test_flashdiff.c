#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashdiff.h"
#include "../src/flashlayout.h"

static void build_layout_a(FlashLayout *layout) {
    flashlayout_init(layout);
    FlashRegion r;
    /* .text: 0x0800_0000, 32 KB */
    strncpy(r.name, ".text", sizeof(r.name) - 1);
    r.address = 0x08000000; r.size = 32768;
    flashlayout_add(layout, &r);
    /* .data: 0x2000_0000, 4 KB */
    strncpy(r.name, ".data", sizeof(r.name) - 1);
    r.address = 0x20000000; r.size = 4096;
    flashlayout_add(layout, &r);
    /* .bss: 0x2000_1000, 8 KB */
    strncpy(r.name, ".bss", sizeof(r.name) - 1);
    r.address = 0x20001000; r.size = 8192;
    flashlayout_add(layout, &r);
}

static void build_layout_b(FlashLayout *layout) {
    flashlayout_init(layout);
    FlashRegion r;
    /* .text grown to 40 KB */
    strncpy(r.name, ".text", sizeof(r.name) - 1);
    r.address = 0x08000000; r.size = 40960;
    flashlayout_add(layout, &r);
    /* .data unchanged */
    strncpy(r.name, ".data", sizeof(r.name) - 1);
    r.address = 0x20000000; r.size = 4096;
    flashlayout_add(layout, &r);
    /* .bss removed, .heap added */
    strncpy(r.name, ".heap", sizeof(r.name) - 1);
    r.address = 0x20002000; r.size = 2048;
    flashlayout_add(layout, &r);
}

static void test_diff_counts(void) {
    FlashLayout a, b;
    build_layout_a(&a);
    build_layout_b(&b);

    FlashDiff diff;
    int rc = flashdiff_compare(&a, &b, &diff);
    assert(rc == 0);
    /* Entries: .text(MODIFIED), .data(UNCHANGED), .bss(REMOVED), .heap(ADDED) */
    assert(diff.count == 4);

    int found_added = 0, found_removed = 0, found_modified = 0, found_unchanged = 0;
    for (size_t i = 0; i < diff.count; i++) {
        if (diff.entries[i].type == DIFF_ADDED)     found_added++;
        if (diff.entries[i].type == DIFF_REMOVED)   found_removed++;
        if (diff.entries[i].type == DIFF_MODIFIED)  found_modified++;
        if (diff.entries[i].type == DIFF_UNCHANGED) found_unchanged++;
    }
    assert(found_added    == 1);
    assert(found_removed  == 1);
    assert(found_modified == 1);
    assert(found_unchanged== 1);

    flashdiff_free(&diff);
    flashlayout_free(&a);
    flashlayout_free(&b);
    printf("PASS: test_diff_counts\n");
}

static void test_diff_size_delta(void) {
    FlashLayout a, b;
    build_layout_a(&a);
    build_layout_b(&b);

    FlashDiff diff;
    flashdiff_compare(&a, &b, &diff);
    /* delta = +8192 (.text) - 8192 (.bss) + 2048 (.heap) = +2048 */
    assert(diff.total_size_delta == 2048);

    flashdiff_free(&diff);
    flashlayout_free(&a);
    flashlayout_free(&b);
    printf("PASS: test_diff_size_delta\n");
}

static void test_identical_layouts(void) {
    FlashLayout a, b;
    build_layout_a(&a);
    build_layout_a(&b);

    FlashDiff diff;
    flashdiff_compare(&a, &b, &diff);
    assert(diff.total_size_delta == 0);
    for (size_t i = 0; i < diff.count; i++)
        assert(diff.entries[i].type == DIFF_UNCHANGED);

    flashdiff_free(&diff);
    flashlayout_free(&a);
    flashlayout_free(&b);
    printf("PASS: test_identical_layouts\n");
}

int main(void) {
    test_diff_counts();
    test_diff_size_delta();
    test_identical_layouts();
    printf("All flashdiff tests passed.\n");
    return 0;
}
