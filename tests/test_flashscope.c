#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashscope.h"
#include "../src/flashlayout.h"

static void test_init(void) {
    FlashScopeSet set;
    flashscope_init(&set);
    assert(set.count == 0);
    printf("[PASS] test_init\n");
}

static void test_add_and_find(void) {
    FlashScopeSet set;
    flashscope_init(&set);

    assert(flashscope_add(&set, "bootloader", 0x08000000, 0x8000) == 0);
    assert(flashscope_add(&set, "application", 0x08008000, 0x70000) == 0);
    assert(set.count == 2);

    FlashScope *s = flashscope_find(&set, "bootloader");
    assert(s != NULL);
    assert(s->base_addr == 0x08000000);
    assert(s->size == 0x8000);
    assert(s->active == true);

    assert(flashscope_find(&set, "nonexistent") == NULL);
    printf("[PASS] test_add_and_find\n");
}

static void test_contains(void) {
    FlashScopeSet set;
    flashscope_init(&set);
    flashscope_add(&set, "boot", 0x08000000, 0x4000);

    assert(flashscope_contains(&set, 0x08000000) == true);
    assert(flashscope_contains(&set, 0x08003FFF) == true);
    assert(flashscope_contains(&set, 0x08004000) == false);
    assert(flashscope_contains(&set, 0x00000000) == false);
    printf("[PASS] test_contains\n");
}

static void test_set_active(void) {
    FlashScopeSet set;
    flashscope_init(&set);
    flashscope_add(&set, "region_a", 0x08000000, 0x1000);

    assert(flashscope_contains(&set, 0x08000100) == true);
    assert(flashscope_set_active(&set, "region_a", false) == 0);
    assert(flashscope_contains(&set, 0x08000100) == false);
    assert(flashscope_set_active(&set, "region_a", true) == 0);
    assert(flashscope_contains(&set, 0x08000100) == true);

    assert(flashscope_set_active(&set, "missing", false) == -1);
    printf("[PASS] test_set_active\n");
}

static void test_filter_layout(void) {
    FlashScopeSet set;
    flashscope_init(&set);
    flashscope_add(&set, "boot", 0x08000000, 0x8000);

    FlashLayout layout, out;
    flashlayout_init(&layout);

    FlashRegion r1 = { .name = "boot_code",  .start_addr = 0x08000000, .size = 0x4000 };
    FlashRegion r2 = { .name = "app_code",   .start_addr = 0x08010000, .size = 0x20000 };
    FlashRegion r3 = { .name = "boot_data",  .start_addr = 0x08006000, .size = 0x1000 };

    flashlayout_add_region(&layout, &r1);
    flashlayout_add_region(&layout, &r2);
    flashlayout_add_region(&layout, &r3);

    int n = flashscope_filter_layout(&set, &layout, &out);
    assert(n == 2);
    assert(out.count == 2);
    assert(strncmp(out.regions[0].name, "boot_code", 16) == 0);
    assert(strncmp(out.regions[1].name, "boot_data", 16) == 0);
    printf("[PASS] test_filter_layout\n");
}

static void test_add_null_guard(void) {
    FlashScopeSet set;
    flashscope_init(&set);
    assert(flashscope_add(&set, NULL, 0x0, 0x100) == -1);
    assert(flashscope_add(NULL, "x", 0x0, 0x100) == -1);
    printf("[PASS] test_add_null_guard\n");
}

int main(void) {
    printf("=== flashscope tests ===\n");
    test_init();
    test_add_and_find();
    test_contains();
    test_set_active();
    test_filter_layout();
    test_add_null_guard();
    printf("All flashscope tests passed.\n");
    return 0;
}
