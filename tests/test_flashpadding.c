#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/flashpadding.h"
#include "../src/flashregion.h"

/* Helper: build a minimal FlashRegion array on the stack */
static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    return r;
}

static void test_no_padding_needed(void) {
    FlashRegion regions[2];
    regions[0] = make_region("A", 0x0000, 0x100);
    regions[1] = make_region("B", 0x0100, 0x100); /* immediately adjacent */

    FlashPaddingResult r = flash_padding_compute(regions, 2, 0x100,
                                                  FLASH_PAD_ZERO, 0x00);
    assert(r.count == 0);
    assert(r.total_padding == 0);
    flash_padding_result_free(&r);
    printf("PASS: test_no_padding_needed\n");
}

static void test_single_gap(void) {
    FlashRegion regions[2];
    regions[0] = make_region("A", 0x0000, 0x0090); /* ends at 0x90 */
    regions[1] = make_region("B", 0x0100, 0x0100); /* starts at 0x100 */

    /* alignment=0x100 => aligned_end of region A = 0x100, gap = 0x90..0x100 */
    FlashPaddingResult r = flash_padding_compute(regions, 2, 0x100,
                                                  FLASH_PAD_FF, 0xFF);
    assert(r.count == 1);
    assert(r.regions[0].start == 0x0090);
    assert(r.regions[0].end   == 0x0100);
    assert(r.regions[0].size  == 0x0070);
    assert(r.regions[0].fill  == FLASH_PAD_FF);
    assert(r.total_padding    == 0x0070);
    flash_padding_result_free(&r);
    printf("PASS: test_single_gap\n");
}

static void test_pattern_fill(void) {
    FlashRegion regions[2];
    regions[0] = make_region("X", 0x1000, 0x0030);
    regions[1] = make_region("Y", 0x1080, 0x0080);

    FlashPaddingResult r = flash_padding_compute(regions, 2, 0x40,
                                                  FLASH_PAD_PATTERN, 0xAB);
    assert(r.count == 1);
    assert(r.regions[0].fill    == FLASH_PAD_PATTERN);
    assert(r.regions[0].pattern == 0xAB);
    assert(r.regions[0].start   == 0x1030);
    assert(r.regions[0].end     == 0x1040);
    assert(r.regions[0].size    == 0x10);
    flash_padding_result_free(&r);
    printf("PASS: test_pattern_fill\n");
}

static void test_multiple_gaps(void) {
    FlashRegion regions[4];
    regions[0] = make_region("A", 0x0000, 0x0050);
    regions[1] = make_region("B", 0x0100, 0x0050);
    regions[2] = make_region("C", 0x0200, 0x0050);
    regions[3] = make_region("D", 0x0300, 0x0050);

    FlashPaddingResult r = flash_padding_compute(regions, 4, 0x100,
                                                  FLASH_PAD_ZERO, 0x00);
    assert(r.count == 3);
    assert(r.total_padding == 3 * 0xB0); /* each gap: 0x50..0x100 = 0xB0 */
    flash_padding_result_free(&r);
    printf("PASS: test_multiple_gaps\n");
}

static void test_empty_input(void) {
    FlashPaddingResult r = flash_padding_compute(NULL, 0, 0x100,
                                                  FLASH_PAD_ZERO, 0x00);
    assert(r.count == 0);
    assert(r.total_padding == 0);
    flash_padding_result_free(&r);
    printf("PASS: test_empty_input\n");
}

static void test_total_helper(void) {
    FlashRegion regions[2];
    regions[0] = make_region("A", 0x0000, 0x0060);
    regions[1] = make_region("B", 0x0100, 0x0040);

    FlashPaddingResult r = flash_padding_compute(regions, 2, 0x80,
                                                  FLASH_PAD_ZERO, 0x00);
    assert(flash_padding_total(&r) == r.total_padding);
    assert(flash_padding_total(NULL) == 0);
    flash_padding_result_free(&r);
    printf("PASS: test_total_helper\n");
}

int main(void) {
    test_no_padding_needed();
    test_single_gap();
    test_pattern_fill();
    test_multiple_gaps();
    test_empty_input();
    test_total_helper();
    printf("All flashpadding tests passed.\n");
    return 0;
}
