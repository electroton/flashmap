#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/flashgroup.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, const char *tag,
                               uint32_t start, uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    strncpy(r.tag,  tag,  sizeof(r.tag)  - 1);
    r.start = start;
    r.size  = size;
    r.used  = used;
    return r;
}

static void test_groupset_init(void) {
    FlashGroupSet gs;
    flash_groupset_init(&gs);
    assert(gs.count == 0);
    printf("  [PASS] groupset_init\n");
}

static void test_group_by_tag(void) {
    FlashRegion regions[] = {
        make_region("bootloader", "boot",  0x0000, 0x4000, 0x3800),
        make_region("app_code",   "app",   0x4000, 0x8000, 0x6000),
        make_region("app_data",   "app",   0xC000, 0x2000, 0x1000),
        make_region("nvs",        "config",0xE000, 0x1000, 0x0200),
    };
    size_t n = sizeof(regions) / sizeof(regions[0]);

    FlashGroupSet gs;
    flash_groupset_init(&gs);
    int rc = flash_group_regions(&gs, regions, n, FLASH_GROUP_BY_TAG);
    assert(rc == 0);
    assert(gs.count == 3); /* boot, app, config */

    FlashGroup *app = flash_group_find(&gs, "app");
    assert(app != NULL);
    assert(app->count == 2);
    assert(app->total_size == 0x8000 + 0x2000);
    assert(app->total_used == 0x6000 + 0x1000);

    FlashGroup *boot = flash_group_find(&gs, "boot");
    assert(boot != NULL);
    assert(boot->count == 1);

    printf("  [PASS] group_by_tag\n");
}

static void test_group_find_missing(void) {
    FlashGroupSet gs;
    flash_groupset_init(&gs);
    FlashGroup *g = flash_group_find(&gs, "nonexistent");
    assert(g == NULL);
    printf("  [PASS] group_find_missing\n");
}

static void test_group_by_tag_single(void) {
    FlashRegion regions[] = {
        make_region("only", "sole", 0x0000, 0x1000, 0x0800),
    };
    FlashGroupSet gs;
    flash_groupset_init(&gs);
    assert(flash_group_regions(&gs, regions, 1, FLASH_GROUP_BY_TAG) == 0);
    assert(gs.count == 1);
    FlashGroup *g = flash_group_find(&gs, "sole");
    assert(g != NULL);
    assert(g->total_size == 0x1000);
    printf("  [PASS] group_by_tag_single\n");
}

static void test_group_empty_input(void) {
    FlashGroupSet gs;
    flash_groupset_init(&gs);
    assert(flash_group_regions(&gs, NULL, 0, FLASH_GROUP_BY_TAG) == 0);
    assert(gs.count == 0);
    printf("  [PASS] group_empty_input\n");
}

int main(void) {
    printf("Running flashgroup tests...\n");
    test_groupset_init();
    test_group_by_tag();
    test_group_find_missing();
    test_group_by_tag_single();
    test_group_empty_input();
    printf("All flashgroup tests passed.\n");
    return 0;
}
