#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashmark.h"

static void test_init(void)
{
    FlashMarkSet set;
    flashmark_init(&set);
    assert(set.count == 0);
    printf("[PASS] test_init\n");
}

static void test_add_and_find(void)
{
    FlashMarkSet set;
    flashmark_init(&set);

    assert(flashmark_add(&set, "boot_start", 0x08000000,
                         FLASHMARK_TYPE_START, 0) == 0);
    assert(flashmark_add(&set, "app_end",    0x0807FFFF,
                         FLASHMARK_TYPE_END,   1) == 0);
    assert(set.count == 2);

    const FlashMark *m = flashmark_find(&set, "boot_start");
    assert(m != NULL);
    assert(m->address == 0x08000000);
    assert(m->type    == FLASHMARK_TYPE_START);

    assert(flashmark_find(&set, "nonexistent") == NULL);
    printf("[PASS] test_add_and_find\n");
}

static void test_duplicate_rejected(void)
{
    FlashMarkSet set;
    flashmark_init(&set);
    assert(flashmark_add(&set, "mark", 0x1000, FLASHMARK_TYPE_CUSTOM, -1) == 0);
    assert(flashmark_add(&set, "mark", 0x2000, FLASHMARK_TYPE_CUSTOM, -1) == -1);
    assert(set.count == 1);
    printf("[PASS] test_duplicate_rejected\n");
}

static void test_remove(void)
{
    FlashMarkSet set;
    flashmark_init(&set);
    flashmark_add(&set, "a", 0x100, FLASHMARK_TYPE_CUSTOM, -1);
    flashmark_add(&set, "b", 0x200, FLASHMARK_TYPE_CUSTOM, -1);
    flashmark_add(&set, "c", 0x300, FLASHMARK_TYPE_CUSTOM, -1);

    assert(flashmark_remove(&set, "b") == 0);
    assert(set.count == 2);
    assert(flashmark_find(&set, "b") == NULL);
    assert(flashmark_find(&set, "a") != NULL);
    assert(flashmark_find(&set, "c") != NULL);

    assert(flashmark_remove(&set, "missing") == -1);
    printf("[PASS] test_remove\n");
}

static void test_in_range(void)
{
    FlashMarkSet set;
    flashmark_init(&set);
    flashmark_add(&set, "m1", 0x1000, FLASHMARK_TYPE_CUSTOM, -1);
    flashmark_add(&set, "m2", 0x2000, FLASHMARK_TYPE_CUSTOM, -1);
    flashmark_add(&set, "m3", 0x3000, FLASHMARK_TYPE_CUSTOM, -1);

    const FlashMark *results[8];
    size_t n = flashmark_in_range(&set, 0x1000, 0x2000, results, 8);
    assert(n == 2); /* 0x1000 and 0x2000 are in [0x1000, 0x3000) */
    printf("[PASS] test_in_range\n");
}

static void test_sort(void)
{
    FlashMarkSet set;
    flashmark_init(&set);
    flashmark_add(&set, "z", 0x3000, FLASHMARK_TYPE_CUSTOM, -1);
    flashmark_add(&set, "a", 0x1000, FLASHMARK_TYPE_CUSTOM, -1);
    flashmark_add(&set, "m", 0x2000, FLASHMARK_TYPE_CUSTOM, -1);

    flashmark_sort(&set);
    assert(set.marks[0].address == 0x1000);
    assert(set.marks[1].address == 0x2000);
    assert(set.marks[2].address == 0x3000);
    printf("[PASS] test_sort\n");
}

int main(void)
{
    test_init();
    test_add_and_find();
    test_duplicate_rejected();
    test_remove();
    test_in_range();
    test_sort();
    printf("All flashmark tests passed.\n");
    return 0;
}
