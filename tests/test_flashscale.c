#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashscale.h"
#include "../src/flashregion.h"
#include "../src/flashlayout.h"

/* Helpers */
static FlashRegion make_region(const char *name, uint32_t offset,
                                uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.offset = offset;
    r.size   = size;
    r.used   = used;
    return r;
}

static void test_scale_single_region(void) {
    FlashRegion r = make_region("bootloader", 0x0000, 0x4000, 0x2000);
    FlashScaledRegion scaled;

    int ret = flashscale_region(&r, 0x10000, &scaled);
    assert(ret == 0);
    assert(scaled.region == &r);

    /* offset_pct: 0 / 65536 = 0.0 */
    assert(scaled.offset_pct >= 0.0 && scaled.offset_pct < 0.001);

    /* size_pct: 16384 / 65536 = 25.0 */
    assert(scaled.size_pct > 24.9 && scaled.size_pct < 25.1);

    /* used_pct: 8192 / 16384 = 50.0 */
    assert(scaled.used_pct > 49.9 && scaled.used_pct < 50.1);

    printf("PASS: test_scale_single_region\n");
}

static void test_scale_region_zero_size(void) {
    FlashRegion r = make_region("empty", 0x1000, 0, 0);
    FlashScaledRegion scaled;

    int ret = flashscale_region(&r, 0x10000, &scaled);
    assert(ret == 0);
    assert(scaled.size_pct >= 0.0 && scaled.size_pct < 0.001);
    assert(scaled.used_pct >= 0.0 && scaled.used_pct < 0.001);

    printf("PASS: test_scale_region_zero_size\n");
}

static void test_scale_layout(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));

    layout.regions[0] = make_region("boot",   0x00000, 0x08000, 0x06000);
    layout.regions[1] = make_region("app",    0x08000, 0x30000, 0x20000);
    layout.regions[2] = make_region("config", 0x38000, 0x08000, 0x01000);
    layout.count = 3;
    layout.total_size = 0x40000;

    FlashScaleResult result;
    int ret = flashscale_layout(&layout, &result);
    assert(ret == 0);
    assert(result.count == 3);
    assert(result.total_capacity == 0x40000);

    /* boot: size 0x8000 / 0x40000 = 12.5% */
    assert(result.entries[0].size_pct > 12.4 && result.entries[0].size_pct < 12.6);

    /* app: size 0x30000 / 0x40000 = 75% */
    assert(result.entries[1].size_pct > 74.9 && result.entries[1].size_pct < 75.1);

    /* overall used: (0x6000 + 0x20000 + 0x1000) / 0x40000 */
    double expected_overall = (double)(0x6000 + 0x20000 + 0x1000) / (double)0x40000 * 100.0;
    assert(result.overall_used_pct > expected_overall - 0.1 &&
           result.overall_used_pct < expected_overall + 0.1);

    flashscale_free(&result);
    printf("PASS: test_scale_layout\n");
}

static void test_scale_null_inputs(void) {
    FlashScaledRegion scaled;
    assert(flashscale_region(NULL, 0x1000, &scaled) == -1);
    assert(flashscale_region(NULL, 0,      &scaled) == -1);

    FlashScaleResult result;
    assert(flashscale_layout(NULL, &result) == -1);

    printf("PASS: test_scale_null_inputs\n");
}

int main(void) {
    test_scale_single_region();
    test_scale_region_zero_size();
    test_scale_layout();
    test_scale_null_inputs();
    printf("All flashscale tests passed.\n");
    return 0;
}
