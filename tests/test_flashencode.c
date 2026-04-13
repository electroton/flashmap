#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashencode.h"
#include "../src/flashencode_report.h"
#include "../src/flashlayout.h"

static void test_encode_hex_basic(void) {
    const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    char out[16];
    FlashEncodeResult r = flash_encode_hex(data, 4, out, sizeof(out));
    assert(r == FLASH_ENCODE_OK);
    assert(strcmp(out, "deadbeef") == 0);
    printf("PASS: test_encode_hex_basic\n");
}

static void test_encode_hex_empty(void) {
    const uint8_t data[] = {0x00};
    char out[4];
    FlashEncodeResult r = flash_encode_hex(data, 1, out, sizeof(out));
    assert(r == FLASH_ENCODE_OK);
    assert(strcmp(out, "00") == 0);
    printf("PASS: test_encode_hex_empty\n");
}

static void test_encode_hex_buffer_too_small(void) {
    const uint8_t data[] = {0xAB, 0xCD};
    char out[3]; /* needs 5 */
    FlashEncodeResult r = flash_encode_hex(data, 2, out, sizeof(out));
    assert(r == FLASH_ENCODE_ERR_BUFFER);
    printf("PASS: test_encode_hex_buffer_too_small\n");
}

static void test_encode_base64_basic(void) {
    /* "Man" -> "TWFu" */
    const uint8_t data[] = {'M', 'a', 'n'};
    char out[16];
    FlashEncodeResult r = flash_encode_base64(data, 3, out, sizeof(out));
    assert(r == FLASH_ENCODE_OK);
    assert(strcmp(out, "TWFu") == 0);
    printf("PASS: test_encode_base64_basic\n");
}

static void test_encode_base64_padding(void) {
    /* "Ma" -> "TWE=" */
    const uint8_t data[] = {'M', 'a'};
    char out[16];
    FlashEncodeResult r = flash_encode_base64(data, 2, out, sizeof(out));
    assert(r == FLASH_ENCODE_OK);
    assert(strcmp(out, "TWE=") == 0);
    printf("PASS: test_encode_base64_padding\n");
}

static void test_encode_base64_null(void) {
    char out[16];
    FlashEncodeResult r = flash_encode_base64(NULL, 4, out, sizeof(out));
    assert(r == FLASH_ENCODE_ERR_NULL);
    printf("PASS: test_encode_base64_null\n");
}

static void test_encode_region_hex(void) {
    FlashRegion reg;
    strncpy(reg.name, "BOOT", sizeof(reg.name));
    reg.start = 0x08000000;
    reg.size  = 0x4000;
    char out[256];
    FlashEncodeResult r = flash_encode_region(&reg, FLASH_ENCODE_FMT_HEX, out, sizeof(out));
    assert(r == FLASH_ENCODE_OK);
    assert(strlen(out) > 0);
    printf("PASS: test_encode_region_hex: %s\n", out);
}

static void test_encode_region_base64(void) {
    FlashRegion reg;
    strncpy(reg.name, "APP", sizeof(reg.name));
    reg.start = 0x08004000;
    reg.size  = 0x10000;
    char out[256];
    FlashEncodeResult r = flash_encode_region(&reg, FLASH_ENCODE_FMT_BASE64, out, sizeof(out));
    assert(r == FLASH_ENCODE_OK);
    assert(strlen(out) > 0);
    printf("PASS: test_encode_region_base64: %s\n", out);
}

static void test_encode_region_bad_format(void) {
    FlashRegion reg;
    strncpy(reg.name, "X", sizeof(reg.name));
    reg.start = 0; reg.size = 1;
    char out[256];
    FlashEncodeResult r = flash_encode_region(&reg, (FlashEncodeFormat)99, out, sizeof(out));
    assert(r == FLASH_ENCODE_ERR_FORMAT);
    printf("PASS: test_encode_region_bad_format\n");
}

static void test_encode_report(void) {
    FlashLayout layout;
    flash_layout_init(&layout);
    FlashRegion r1, r2;
    strncpy(r1.name, "BOOT", sizeof(r1.name)); r1.start = 0x0; r1.size = 0x1000;
    strncpy(r2.name, "APP",  sizeof(r2.name)); r2.start = 0x1000; r2.size = 0x8000;
    flash_layout_add(&layout, &r1);
    flash_layout_add(&layout, &r2);
    printf("--- Encode Report (hex) ---\n");
    flash_encode_report_layout(&layout, FLASH_ENCODE_FMT_HEX, stdout);
    printf("PASS: test_encode_report\n");
}

int main(void) {
    test_encode_hex_basic();
    test_encode_hex_empty();
    test_encode_hex_buffer_too_small();
    test_encode_base64_basic();
    test_encode_base64_padding();
    test_encode_base64_null();
    test_encode_region_hex();
    test_encode_region_base64();
    test_encode_region_bad_format();
    test_encode_report();
    printf("All flashencode tests passed.\n");
    return 0;
}
