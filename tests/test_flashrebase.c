#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashrebase.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* ---- helpers ---- */
static FlashLayout make_layout(uint32_t base)
{
    FlashLayout l;
    l.count = 3;
    l.regions = malloc(3 * sizeof(FlashRegion));
    assert(l.regions);
    l.regions[0] = (FlashRegion){ .name = "boot",  .start = base,        .size = 0x4000 };
    l.regions[1] = (FlashRegion){ .name = "app",   .start = base+0x4000, .size = 0x8000 };
    l.regions[2] = (FlashRegion){ .name = "config", .start = base+0xC000, .size = 0x1000 };
    return l;
}

static void free_src_layout(FlashLayout *l)
{
    free(l->regions);
    l->regions = NULL;
    l->count = 0;
}

/* ---- tests ---- */

static void test_rebase_basic(void)
{
    FlashLayout src = make_layout(0x08000000);
    FlashLayout out = {0};
    FlashRebaseOptions opts = { .check_overflow = true, .copy_names = false };

    FlashRebaseResult r = flash_rebase(&src, 0x20000000, opts, &out);
    assert(r == FLASH_REBASE_OK);
    assert(out.count == 3);
    assert(out.regions[0].start == 0x20000000);
    assert(out.regions[1].start == 0x20004000);
    assert(out.regions[2].start == 0x2000C000);
    /* sizes must be unchanged */
    assert(out.regions[0].size == 0x4000);
    assert(out.regions[1].size == 0x8000);

    flash_rebase_layout_free(&out, false);
    free_src_layout(&src);
    printf("PASS test_rebase_basic\n");
}

static void test_rebase_to_zero(void)
{
    FlashLayout src = make_layout(0x08000000);
    FlashLayout out = {0};
    FlashRebaseOptions opts = { .check_overflow = false, .copy_names = false };

    FlashRebaseResult r = flash_rebase(&src, 0x00000000, opts, &out);
    assert(r == FLASH_REBASE_OK);
    assert(out.regions[0].start == 0x00000000);
    assert(out.regions[2].start == 0x0000C000);

    flash_rebase_layout_free(&out, false);
    free_src_layout(&src);
    printf("PASS test_rebase_to_zero\n");
}

static void test_rebase_overflow_detected(void)
{
    FlashLayout src = make_layout(0xFFFF0000);
    FlashLayout out = {0};
    FlashRebaseOptions opts = { .check_overflow = true, .copy_names = false };

    /* Rebasing to 0xFFFF0000 + some positive delta should overflow */
    FlashRebaseResult r = flash_rebase(&src, 0xFFFF8000, opts, &out);
    assert(r == FLASH_REBASE_ERR_OVERFLOW);
    assert(out.regions == NULL);

    free_src_layout(&src);
    printf("PASS test_rebase_overflow_detected\n");
}

static void test_rebase_null_args(void)
{
    FlashLayout out = {0};
    FlashRebaseOptions opts = { .check_overflow = false, .copy_names = false };
    assert(flash_rebase(NULL, 0, opts, &out) == FLASH_REBASE_ERR_NULL);
    assert(flash_rebase(NULL, 0, opts, NULL) == FLASH_REBASE_ERR_NULL);
    printf("PASS test_rebase_null_args\n");
}

static void test_rebase_empty_layout(void)
{
    FlashLayout src = { .count = 0, .regions = NULL };
    FlashLayout out = {0};
    FlashRebaseOptions opts = { .check_overflow = true, .copy_names = false };

    FlashRebaseResult r = flash_rebase(&src, 0x1000, opts, &out);
    assert(r == FLASH_REBASE_OK);
    assert(out.count == 0);
    assert(out.regions == NULL);
    printf("PASS test_rebase_empty_layout\n");
}

static void test_result_str(void)
{
    assert(strcmp(flash_rebase_result_str(FLASH_REBASE_OK), "OK") == 0);
    assert(strcmp(flash_rebase_result_str(FLASH_REBASE_ERR_OVERFLOW), "Address overflow") == 0);
    printf("PASS test_result_str\n");
}

int main(void)
{
    test_rebase_basic();
    test_rebase_to_zero();
    test_rebase_overflow_detected();
    test_rebase_null_args();
    test_rebase_empty_layout();
    test_result_str();
    printf("All flashrebase tests passed.\n");
    return 0;
}
