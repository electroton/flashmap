#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/flashrenumber.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* Helper: build a small layout with N regions */
static FlashLayout *make_layout(void) {
    FlashLayout *l = flash_layout_create();
    assert(l);

    FlashRegion r;
    memset(&r, 0, sizeof(r));

    r.start = 0x0000; r.size = 0x1000; snprintf(r.name, sizeof(r.name), "boot");
    flash_layout_add_region(l, &r);

    r.start = 0x1000; r.size = 0x2000; snprintf(r.name, sizeof(r.name), "app");
    flash_layout_add_region(l, &r);

    r.start = 0x3000; r.size = 0x0800; snprintf(r.name, sizeof(r.name), "data");
    flash_layout_add_region(l, &r);

    return l;
}

static void test_shift_mode(void) {
    FlashLayout *l = make_layout();
    FlashRenumberOptions opts = {
        .mode      = RENUMBER_SHIFT,
        .base      = 0x8000000,
        .alignment = 0,
        .dry_run   = false
    };
    FlashRenumberResult res;
    int rc = flash_renumber(l, &opts, &res);
    assert(rc == 0);
    assert(res.regions_renumbered == 3);
    assert(res.min_new_addr == 0x8000000);
    assert(res.max_new_addr == 0x8000000 + 0x3000);

    /* Verify first region shifted correctly */
    const FlashRegion *boot = flash_layout_get_region(l, 0);
    assert(boot && boot->start == 0x8000000);

    const FlashRegion *app = flash_layout_get_region(l, 1);
    assert(app && app->start == 0x8001000);

    flash_layout_destroy(l);
    printf("PASS: test_shift_mode\n");
}

static void test_sequential_mode(void) {
    FlashLayout *l = make_layout();
    FlashRenumberOptions opts = {
        .mode      = RENUMBER_SEQUENTIAL,
        .base      = 0x0000,
        .alignment = 0x100,
        .dry_run   = false
    };
    FlashRenumberResult res;
    int rc = flash_renumber(l, &opts, &res);
    assert(rc == 0);
    assert(res.regions_renumbered == 3);

    const FlashRegion *boot = flash_layout_get_region(l, 0);
    assert(boot && boot->start == 0x0000);

    const FlashRegion *app = flash_layout_get_region(l, 1);
    assert(app && app->start == 0x1000); /* 0x0000 + 0x1000 aligned to 0x100 */

    const FlashRegion *data = flash_layout_get_region(l, 2);
    assert(data && data->start == 0x3000); /* 0x1000 + 0x2000 */

    flash_layout_destroy(l);
    printf("PASS: test_sequential_mode\n");
}

static void test_dry_run_no_modify(void) {
    FlashLayout *l = make_layout();
    FlashRenumberOptions opts = {
        .mode      = RENUMBER_SHIFT,
        .base      = 0x4000,
        .alignment = 0,
        .dry_run   = true
    };
    FlashRenumberResult res;
    int rc = flash_renumber(l, &opts, &res);
    assert(rc == 0);
    assert(res.regions_renumbered == 3);

    /* Original addresses must be unchanged */
    const FlashRegion *boot = flash_layout_get_region(l, 0);
    assert(boot && boot->start == 0x0000);

    flash_layout_destroy(l);
    printf("PASS: test_dry_run_no_modify\n");
}

static void test_null_result_allowed(void) {
    FlashLayout *l = make_layout();
    FlashRenumberOptions opts = {
        .mode      = RENUMBER_SHIFT,
        .base      = 0x100,
        .alignment = 0,
        .dry_run   = false
    };
    int rc = flash_renumber(l, &opts, NULL);
    assert(rc == 0);
    flash_layout_destroy(l);
    printf("PASS: test_null_result_allowed\n");
}

int main(void) {
    test_shift_mode();
    test_sequential_mode();
    test_dry_run_no_modify();
    test_null_result_allowed();
    printf("All flashrenumber tests passed.\n");
    return 0;
}
