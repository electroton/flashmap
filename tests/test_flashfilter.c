#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashfilter.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_test_layout(void)
{
    FlashLayout *layout = flashlayout_create();
    assert(layout);

    FlashRegion r1 = { .name = "FLASH",  .base = 0x08000000, .size = 0x40000, .used = 0x20000 };
    FlashRegion r2 = { .name = "RAM",    .base = 0x20000000, .size = 0x10000, .used = 0x8000  };
    FlashRegion r3 = { .name = "EEPROM", .base = 0x08080000, .size = 0x800,   .used = 0x700   };

    assert(flashlayout_add_region(layout, &r1) == 0);
    assert(flashlayout_add_region(layout, &r2) == 0);
    assert(flashlayout_add_region(layout, &r3) == 0);
    return layout;
}

static void test_filter_no_criteria(void)
{
    FlashLayout *layout = make_test_layout();
    FlashFilter filter;
    flashfilter_init(&filter);

    FlashLayout *result = flashfilter_apply(layout, &filter);
    assert(result);
    assert(result->region_count == 3);

    flashlayout_free(result);
    flashlayout_free(layout);
    printf("PASS: test_filter_no_criteria\n");
}

static void test_filter_by_name(void)
{
    FlashLayout *layout = make_test_layout();
    FlashFilter filter;
    flashfilter_init(&filter);
    filter.flags        = FILTER_BY_NAME;
    filter.name_pattern = "FLASH";

    FlashLayout *result = flashfilter_apply(layout, &filter);
    assert(result);
    assert(result->region_count == 1);
    assert(strcmp(result->regions[0].name, "FLASH") == 0);

    flashlayout_free(result);
    flashlayout_free(layout);
    printf("PASS: test_filter_by_name\n");
}

static void test_filter_by_min_size(void)
{
    FlashLayout *layout = make_test_layout();
    FlashFilter filter;
    flashfilter_init(&filter);
    filter.flags    = FILTER_BY_MIN_SIZE;
    filter.min_size = 0x10000;

    FlashLayout *result = flashfilter_apply(layout, &filter);
    assert(result);
    assert(result->region_count == 2); /* FLASH and RAM qualify */

    flashlayout_free(result);
    flashlayout_free(layout);
    printf("PASS: test_filter_by_min_size\n");
}

static void test_filter_by_usage(void)
{
    FlashLayout *layout = make_test_layout();
    FlashFilter filter;
    flashfilter_init(&filter);
    filter.flags         = FILTER_BY_USAGE;
    filter.min_usage_pct = 80.0f;
    filter.max_usage_pct = 100.0f;

    FlashLayout *result = flashfilter_apply(layout, &filter);
    assert(result);
    /* EEPROM is ~87.5% used; FLASH=50%, RAM=50% */
    assert(result->region_count == 1);
    assert(strcmp(result->regions[0].name, "EEPROM") == 0);

    flashlayout_free(result);
    flashlayout_free(layout);
    printf("PASS: test_filter_by_usage\n");
}

int main(void)
{
    test_filter_no_criteria();
    test_filter_by_name();
    test_filter_by_min_size();
    test_filter_by_usage();
    printf("All flashfilter tests passed.\n");
    return 0;
}
