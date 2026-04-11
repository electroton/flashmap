#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashsearch.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_test_layout(void)
{
    FlashLayout *layout = flash_layout_create();
    assert(layout);

    FlashRegion r;
    r.name = "bootloader"; r.start = 0x08000000; r.size = 0x8000; r.type = 0;
    flash_layout_add_region(layout, &r);

    r.name = "application"; r.start = 0x08008000; r.size = 0x70000; r.type = 0;
    flash_layout_add_region(layout, &r);

    r.name = "config_data"; r.start = 0x08078000; r.size = 0x4000; r.type = 1;
    flash_layout_add_region(layout, &r);

    r.name = "nvs_storage"; r.start = 0x0807C000; r.size = 0x4000; r.type = 1;
    flash_layout_add_region(layout, &r);

    return layout;
}

static void test_search_by_name_substr(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSearchResult result;
    assert(flash_search_result_init(&result) == 0);

    FlashSearchQuery q = {0};
    q.flags = FLASH_SEARCH_BY_NAME;
    q.name_substr = "config";

    int found = flash_search(layout, &q, &result);
    assert(found == 1);
    assert(strcmp(result.matches[0]->name, "config_data") == 0);

    flash_search_result_free(&result);
    flash_layout_destroy(layout);
    printf("PASS: test_search_by_name_substr\n");
}

static void test_search_by_address_range(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSearchResult result;
    assert(flash_search_result_init(&result) == 0);

    FlashSearchQuery q = {0};
    q.flags = FLASH_SEARCH_BY_ADDR;
    q.addr_min = 0x08078000;
    q.addr_max = 0x0807FFFF;

    int found = flash_search(layout, &q, &result);
    assert(found == 2);

    flash_search_result_free(&result);
    flash_layout_destroy(layout);
    printf("PASS: test_search_by_address_range\n");
}

static void test_search_by_type(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSearchResult result;
    assert(flash_search_result_init(&result) == 0);

    FlashSearchQuery q = {0};
    q.flags = FLASH_SEARCH_BY_TYPE;
    q.region_type = 1;

    int found = flash_search(layout, &q, &result);
    assert(found == 2);

    flash_search_result_free(&result);
    flash_layout_destroy(layout);
    printf("PASS: test_search_by_type\n");
}

static void test_search_by_exact_name(void)
{
    FlashLayout *layout = make_test_layout();
    FlashRegion *r = flash_search_by_name(layout, "application");
    assert(r != NULL);
    assert(r->start == 0x08008000);
    assert(flash_search_by_name(layout, "nonexistent") == NULL);
    flash_layout_destroy(layout);
    printf("PASS: test_search_by_exact_name\n");
}

static void test_search_by_address_lookup(void)
{
    FlashLayout *layout = make_test_layout();
    FlashRegion *r = flash_search_by_address(layout, 0x08010000);
    assert(r != NULL);
    assert(strcmp(r->name, "application") == 0);
    assert(flash_search_by_address(layout, 0x09000000) == NULL);
    flash_layout_destroy(layout);
    printf("PASS: test_search_by_address_lookup\n");
}

static void test_search_combined_criteria(void)
{
    FlashLayout *layout = make_test_layout();
    FlashSearchResult result;
    assert(flash_search_result_init(&result) == 0);

    FlashSearchQuery q = {0};
    q.flags = FLASH_SEARCH_BY_TYPE | FLASH_SEARCH_BY_SIZE;
    q.region_type = 1;
    q.size_min = 0x4000;
    q.size_max = 0x4000;

    int found = flash_search(layout, &q, &result);
    assert(found == 2);

    flash_search_result_free(&result);
    flash_layout_destroy(layout);
    printf("PASS: test_search_combined_criteria\n");
}

int main(void)
{
    test_search_by_name_substr();
    test_search_by_address_range();
    test_search_by_type();
    test_search_by_exact_name();
    test_search_by_address_lookup();
    test_search_combined_criteria();
    printf("All flashsearch tests passed.\n");
    return 0;
}
