#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/flashhistory.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_layout(uint32_t base, uint32_t size)
{
    FlashLayout layout;
    flashlayout_init(&layout);
    FlashRegion r;
    strncpy(r.name, "FLASH", sizeof(r.name) - 1);
    r.base  = base;
    r.size  = size;
    r.flags = 0;
    flashlayout_add_region(&layout, &r);
    return layout;
}

static void test_init_empty(void)
{
    FlashHistory h;
    flashhistory_init(&h);
    assert(flashhistory_count(&h) == 0);
    assert(flashhistory_get(&h, 0) == NULL);
    printf("  [PASS] test_init_empty\n");
}

static void test_record_and_get(void)
{
    FlashHistory h;
    flashhistory_init(&h);

    FlashLayout l1 = make_layout(0x08000000, 0x20000);
    FlashLayout l2 = make_layout(0x08000000, 0x40000);

    assert(flashhistory_record(&h, &l1, "v1") == 0);
    assert(flashhistory_record(&h, &l2, "v2") == 0);
    assert(flashhistory_count(&h) == 2);

    const FlashHistoryEntry *e0 = flashhistory_get(&h, 0);
    assert(e0 != NULL);
    assert(strcmp(e0->label, "v1") == 0);
    assert(e0->layout.regions[0].size == 0x20000);

    const FlashHistoryEntry *e1 = flashhistory_get(&h, 1);
    assert(e1 != NULL);
    assert(strcmp(e1->label, "v2") == 0);
    assert(e1->layout.regions[0].size == 0x40000);

    printf("  [PASS] test_record_and_get\n");
}

static void test_find_by_label(void)
{
    FlashHistory h;
    flashhistory_init(&h);

    FlashLayout l = make_layout(0x08000000, 0x10000);
    flashhistory_record(&h, &l, "release");
    flashhistory_record(&h, &l, "debug");
    flashhistory_record(&h, &l, "release"); /* second 'release' */

    const FlashHistoryEntry *e = flashhistory_find_by_label(&h, "release");
    assert(e != NULL);
    /* Should return the most recent (index 2) */
    assert(e == flashhistory_get(&h, 2));

    assert(flashhistory_find_by_label(&h, "nonexistent") == NULL);
    printf("  [PASS] test_find_by_label\n");
}

static void test_auto_label(void)
{
    FlashHistory h;
    flashhistory_init(&h);

    FlashLayout l = make_layout(0x0, 0x8000);
    flashhistory_record(&h, &l, NULL);
    flashhistory_record(&h, &l, "");

    assert(strcmp(flashhistory_get(&h, 0)->label, "snapshot_0") == 0);
    assert(strcmp(flashhistory_get(&h, 1)->label, "snapshot_1") == 0);
    printf("  [PASS] test_auto_label\n");
}

static void test_clear(void)
{
    FlashHistory h;
    flashhistory_init(&h);

    FlashLayout l = make_layout(0x0, 0x4000);
    flashhistory_record(&h, &l, "a");
    flashhistory_record(&h, &l, "b");
    assert(flashhistory_count(&h) == 2);

    flashhistory_clear(&h);
    assert(flashhistory_count(&h) == 0);
    assert(flashhistory_get(&h, 0) == NULL);
    printf("  [PASS] test_clear\n");
}

int main(void)
{
    printf("Running flashhistory tests...\n");
    test_init_empty();
    test_record_and_get();
    test_find_by_label();
    test_auto_label();
    test_clear();
    printf("All flashhistory tests passed.\n");
    return 0;
}
