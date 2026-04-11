#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashsort.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_test_layout(void)
{
    FlashLayout *layout = flash_layout_create();
    assert(layout);

    FlashRegion r1 = { .name = "bootloader", .address = 0x08000000, .size = 0x4000, .used = 0x3800 };
    FlashRegion r2 = { .name = "application", .address = 0x08010000, .size = 0x20000, .used = 0x18000 };
    FlashRegion r3 = { .name = "config",      .address = 0x08004000, .size = 0x1000, .used = 0x0200 };
    FlashRegion r4 = { .name = "nvdata",      .address = 0x0803E000, .size = 0x2000, .used = 0x0800 };

    flash_layout_add_region(layout, &r1);
    flash_layout_add_region(layout, &r2);
    flash_layout_add_region(layout, &r3);
    flash_layout_add_region(layout, &r4);
    return layout;
}

static void test_sort_by_address_ascending(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSortOptions opts;
    flash_sort_options_default(&opts);

    assert(flash_sort(layout, &opts) == 0);
    assert(layout->regions[0].address <= layout->regions[1].address);
    assert(layout->regions[1].address <= layout->regions[2].address);
    assert(layout->regions[2].address <= layout->regions[3].address);

    flash_layout_free(layout);
    printf("PASS: test_sort_by_address_ascending\n");
}

static void test_sort_by_size_descending(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSortOptions opts = { FLASH_SORT_BY_SIZE, FLASH_SORT_DESCENDING, 1 };

    assert(flash_sort(layout, &opts) == 0);
    for (int i = 0; i < layout->region_count - 1; i++)
        assert(layout->regions[i].size >= layout->regions[i + 1].size);

    flash_layout_free(layout);
    printf("PASS: test_sort_by_size_descending\n");
}

static void test_sort_by_name(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSortOptions opts = { FLASH_SORT_BY_NAME, FLASH_SORT_ASCENDING, 1 };

    assert(flash_sort(layout, &opts) == 0);
    for (int i = 0; i < layout->region_count - 1; i++)
        assert(strcmp(layout->regions[i].name, layout->regions[i + 1].name) <= 0);

    flash_layout_free(layout);
    printf("PASS: test_sort_by_name\n");
}

static void test_sort_copy_does_not_modify_original(void)
{
    FlashLayout *layout = make_test_layout();
    uint32_t first_addr = layout->regions[0].address;

    FlashSortOptions opts;
    flash_sort_options_default(&opts);
    FlashLayout *sorted = flash_sort_copy(layout, &opts);

    assert(sorted);
    assert(layout->regions[0].address == first_addr); /* original unchanged */
    assert(sorted->region_count == layout->region_count);

    flash_layout_free(layout);
    flash_layout_free(sorted);
    printf("PASS: test_sort_copy_does_not_modify_original\n");
}

static void test_sort_null_inputs(void)
{
    FlashSortOptions opts;
    flash_sort_options_default(&opts);
    assert(flash_sort(NULL, &opts) == -1);
    assert(flash_sort_copy(NULL, &opts) == NULL);
    printf("PASS: test_sort_null_inputs\n");
}

int main(void)
{
    test_sort_by_address_ascending();
    test_sort_by_size_descending();
    test_sort_by_name();
    test_sort_copy_does_not_modify_original();
    test_sort_null_inputs();
    printf("All flashsort tests passed.\n");
    return 0;
}
