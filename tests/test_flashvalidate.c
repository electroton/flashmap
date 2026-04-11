#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/flashvalidate.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* ------------------------------------------------------------------ helpers */

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    return r;
}

/* ------------------------------------------------------------------ tests */

static void test_validate_region_ok(void) {
    FlashRegion r = make_region("text", 0x08000000, 0x10000);
    assert(flash_validate_region(&r) == VALIDATE_OK);
    printf("PASS: test_validate_region_ok\n");
}

static void test_validate_region_null(void) {
    assert(flash_validate_region(NULL) == VALIDATE_ERR_NULL);
    printf("PASS: test_validate_region_null\n");
}

static void test_validate_region_zero_size(void) {
    FlashRegion r = make_region("empty", 0x08000000, 0);
    assert(flash_validate_region(&r) == VALIDATE_ERR_ZERO_SIZE);
    printf("PASS: test_validate_region_zero_size\n");
}

static void test_validate_layout_no_issues(void) {
    FlashLayout layout;
    flash_layout_init(&layout);

    FlashRegion r1 = make_region("text",  0x08000000, 0x8000);
    FlashRegion r2 = make_region("data",  0x08008000, 0x2000);
    flash_layout_add(&layout, &r1);
    flash_layout_add(&layout, &r2);

    ValidationResult res = flash_validate_layout(&layout);
    assert(res.count == 0);

    flash_validate_result_free(&res);
    flash_layout_free(&layout);
    printf("PASS: test_validate_layout_no_issues\n");
}

static void test_validate_layout_overlap(void) {
    FlashLayout layout;
    flash_layout_init(&layout);

    /* r2 starts inside r1 */
    FlashRegion r1 = make_region("text",  0x08000000, 0x10000);
    FlashRegion r2 = make_region("data",  0x08008000, 0x10000);
    flash_layout_add(&layout, &r1);
    flash_layout_add(&layout, &r2);

    ValidationResult res = flash_validate_layout(&layout);
    assert(res.count >= 1);

    bool found_overlap = false;
    for (int i = 0; i < res.count; i++) {
        if (res.issues[i].code == VALIDATE_ERR_OVERLAP) {
            found_overlap = true;
            break;
        }
    }
    assert(found_overlap);

    flash_validate_result_free(&res);
    flash_layout_free(&layout);
    printf("PASS: test_validate_layout_overlap\n");
}

static void test_validate_layout_null(void) {
    ValidationResult res = flash_validate_layout(NULL);
    assert(res.count == 1);
    assert(res.issues[0].code == VALIDATE_ERR_NULL);
    flash_validate_result_free(&res);
    printf("PASS: test_validate_layout_null\n");
}

static void test_error_str(void) {
    assert(strcmp(flash_validate_error_str(VALIDATE_OK), "OK") == 0);
    assert(flash_validate_error_str(VALIDATE_ERR_OVERLAP) != NULL);
    printf("PASS: test_error_str\n");
}

/* ------------------------------------------------------------------ main */

int main(void) {
    test_validate_region_ok();
    test_validate_region_null();
    test_validate_region_zero_size();
    test_validate_layout_no_issues();
    test_validate_layout_overlap();
    test_validate_layout_null();
    test_error_str();
    printf("All flashvalidate tests passed.\n");
    return 0;
}
