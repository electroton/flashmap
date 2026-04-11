#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashmerge.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_layout(void) {
    return flash_layout_create();
}

static void add(FlashLayout *l, const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    flash_layout_add_region(l, &r);
}

static void test_merge_no_overlap(void) {
    FlashLayout *base    = make_layout();
    FlashLayout *overlay = make_layout();
    add(base,    "boot",  0x0000, 0x1000);
    add(overlay, "app",   0x2000, 0x2000);

    FlashMergeResult res = flash_merge_layouts(base, overlay, FLASH_MERGE_STRATEGY_BASE_WINS);
    assert(res.error == FLASH_MERGE_ERR_NONE);
    assert(res.merged != NULL);
    assert(res.merged->region_count == 2);
    assert(res.conflict_count == 0);

    flash_merge_result_free(&res);
    flash_layout_free(base);
    flash_layout_free(overlay);
    printf("PASS test_merge_no_overlap\n");
}

static void test_merge_base_wins(void) {
    FlashLayout *base    = make_layout();
    FlashLayout *overlay = make_layout();
    add(base,    "boot",  0x0000, 0x2000);
    add(overlay, "patch", 0x1000, 0x1000);

    FlashMergeResult res = flash_merge_layouts(base, overlay, FLASH_MERGE_STRATEGY_BASE_WINS);
    assert(res.error == FLASH_MERGE_ERR_NONE);
    assert(res.conflict_count == 1);
    /* overlay region skipped, only base survives */
    assert(res.merged->region_count == 1);

    flash_merge_result_free(&res);
    flash_layout_free(base);
    flash_layout_free(overlay);
    printf("PASS test_merge_base_wins\n");
}

static void test_merge_overlay_wins(void) {
    FlashLayout *base    = make_layout();
    FlashLayout *overlay = make_layout();
    add(base,    "boot",  0x0000, 0x2000);
    add(overlay, "patch", 0x1000, 0x0800);

    FlashMergeResult res = flash_merge_layouts(base, overlay, FLASH_MERGE_STRATEGY_OVERLAY_WINS);
    assert(res.error == FLASH_MERGE_ERR_NONE);
    assert(res.conflict_count == 1);
    assert(res.merged->region_count == 2);

    flash_merge_result_free(&res);
    flash_layout_free(base);
    flash_layout_free(overlay);
    printf("PASS test_merge_overlay_wins\n");
}

static void test_merge_null_input(void) {
    FlashMergeResult res = flash_merge_layouts(NULL, NULL, FLASH_MERGE_STRATEGY_BASE_WINS);
    assert(res.error == FLASH_MERGE_ERR_NULL_INPUT);
    assert(res.merged == NULL);
    printf("PASS test_merge_null_input\n");
}

static void test_error_string(void) {
    assert(strcmp(flash_merge_error_str(FLASH_MERGE_ERR_NONE), "none") == 0);
    assert(strcmp(flash_merge_error_str(FLASH_MERGE_ERR_ALLOC), "allocation failure") == 0);
    printf("PASS test_error_string\n");
}

int main(void) {
    test_merge_no_overlap();
    test_merge_base_wins();
    test_merge_overlay_wins();
    test_merge_null_input();
    test_error_string();
    printf("All flashmerge tests passed.\n");
    return 0;
}
