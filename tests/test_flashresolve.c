#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../src/flashresolve.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_layout(void) {
    static FlashRegion regions[] = {
        { .name = "FLASH",  .start = 0x08000000, .size = 0x80000 },
        { .name = "SRAM",   .start = 0x20000000, .size = 0x20000 },
        { .name = "EEPROM", .start = 0x08080000, .size = 0x01000 },
    };
    FlashLayout layout = { .regions = regions, .count = 3 };
    return layout;
}

static void test_resolve_symbol_found(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_symbol(&layout, "SRAM", &result);
    assert(s == RESOLVE_OK);
    assert(result.address == 0x20000000);
    assert(result.offset  == 0);
    assert(strcmp(result.region_name, "SRAM") == 0);
    printf("PASS: test_resolve_symbol_found\n");
}

static void test_resolve_symbol_not_found(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_symbol(&layout, "BOOTROM", &result);
    assert(s == RESOLVE_NOT_FOUND);
    printf("PASS: test_resolve_symbol_not_found\n");
}

static void test_resolve_address_in_region(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_address(&layout, 0x08001000, &result);
    assert(s == RESOLVE_OK);
    assert(result.address == 0x08001000);
    assert(result.offset  == 0x1000);
    assert(strcmp(result.region_name, "FLASH") == 0);
    printf("PASS: test_resolve_address_in_region\n");
}

static void test_resolve_address_out_of_range(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_address(&layout, 0xDEADBEEF, &result);
    assert(s == RESOLVE_OUT_OF_RANGE);
    printf("PASS: test_resolve_address_out_of_range\n");
}

static void test_resolve_expr_region_plus_offset(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_expr(&layout, "FLASH+0x200", &result);
    assert(s == RESOLVE_OK);
    assert(result.address == 0x08000200);
    assert(result.offset  == 0x200);
    assert(strcmp(result.region_name, "FLASH") == 0);
    printf("PASS: test_resolve_expr_region_plus_offset\n");
}

static void test_resolve_expr_offset_out_of_range(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_expr(&layout, "EEPROM+0x2000", &result);
    assert(s == RESOLVE_OUT_OF_RANGE);
    printf("PASS: test_resolve_expr_offset_out_of_range\n");
}

static void test_resolve_expr_raw_address(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_expr(&layout, "0x20000010", &result);
    assert(s == RESOLVE_OK);
    assert(result.address == 0x20000010);
    assert(result.offset  == 0x10);
    printf("PASS: test_resolve_expr_raw_address\n");
}

static void test_resolve_expr_symbol_fallback(void) {
    FlashLayout layout = make_layout();
    FlashResolveResult result = {0};
    FlashResolveStatus s = flash_resolve_expr(&layout, "EEPROM", &result);
    assert(s == RESOLVE_OK);
    assert(result.address == 0x08080000);
    printf("PASS: test_resolve_expr_symbol_fallback\n");
}

static void test_resolve_status_str(void) {
    assert(strcmp(flash_resolve_status_str(RESOLVE_OK),           "OK")                  == 0);
    assert(strcmp(flash_resolve_status_str(RESOLVE_NOT_FOUND),    "symbol not found")    == 0);
    assert(strcmp(flash_resolve_status_str(RESOLVE_OUT_OF_RANGE), "address out of range") == 0);
    printf("PASS: test_resolve_status_str\n");
}

int main(void) {
    test_resolve_symbol_found();
    test_resolve_symbol_not_found();
    test_resolve_address_in_region();
    test_resolve_address_out_of_range();
    test_resolve_expr_region_plus_offset();
    test_resolve_expr_offset_out_of_range();
    test_resolve_expr_raw_address();
    test_resolve_expr_symbol_fallback();
    test_resolve_status_str();
    printf("All flashresolve tests passed.\n");
    return 0;
}
