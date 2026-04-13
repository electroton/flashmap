#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashencode.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t origin, uint32_t length) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.origin = origin;
    r.length = length;
    return r;
}

static void test_hex_encode_decode(void) {
    FlashRegion region = make_region("FLASH", 0x08000000, 8);
    uint8_t raw[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xFF, 0x12, 0x34};
    FlashEncodeResult enc;
    memset(&enc, 0, sizeof(enc));

    int rc = flash_encode_region(&region, raw, sizeof(raw), FLASH_ENC_HEX, &enc);
    assert(rc == 0);
    assert(enc.encoded != NULL);
    assert(enc.encoded_len == 16); /* 2 hex chars per byte */
    assert(strncmp(enc.encoded, "deadbeef00ff1234", 16) == 0);

    FlashEncodeResult dec;
    memset(&dec, 0, sizeof(dec));
    rc = flash_decode_region(enc.encoded, enc.encoded_len, FLASH_ENC_HEX, &dec);
    assert(rc == 0);
    assert(dec.data_len == sizeof(raw));
    assert(memcmp(dec.data, raw, sizeof(raw)) == 0);

    flash_encode_result_free(&enc);
    flash_encode_result_free(&dec);
    printf("PASS: test_hex_encode_decode\n");
}

static void test_base64_encode_decode(void) {
    FlashRegion region = make_region("SRAM", 0x20000000, 3);
    uint8_t raw[] = {0x4D, 0x61, 0x6E}; /* "Man" */
    FlashEncodeResult enc;
    memset(&enc, 0, sizeof(enc));

    int rc = flash_encode_region(&region, raw, sizeof(raw), FLASH_ENC_BASE64, &enc);
    assert(rc == 0);
    assert(enc.encoded != NULL);
    assert(strcmp(enc.encoded, "TWFu") == 0);

    FlashEncodeResult dec;
    memset(&dec, 0, sizeof(dec));
    rc = flash_decode_region(enc.encoded, enc.encoded_len, FLASH_ENC_BASE64, &dec);
    assert(rc == 0);
    assert(dec.data_len == sizeof(raw));
    assert(memcmp(dec.data, raw, sizeof(raw)) == 0);

    flash_encode_result_free(&enc);
    flash_encode_result_free(&dec);
    printf("PASS: test_base64_encode_decode\n");
}

static void test_binary_encode(void) {
    FlashRegion region = make_region("BOOT", 0x00000000, 4);
    uint8_t raw[] = {0x01, 0x02, 0x03, 0x04};
    FlashEncodeResult enc;
    memset(&enc, 0, sizeof(enc));

    int rc = flash_encode_region(&region, raw, sizeof(raw), FLASH_ENC_BINARY, &enc);
    assert(rc == 0);
    assert(enc.data != NULL);
    assert(enc.data_len == sizeof(raw));
    assert(memcmp(enc.data, raw, sizeof(raw)) == 0);

    flash_encode_result_free(&enc);
    printf("PASS: test_binary_encode\n");
}

static void test_format_name(void) {
    assert(strcmp(flash_encode_format_name(FLASH_ENC_HEX),    "hex")    == 0);
    assert(strcmp(flash_encode_format_name(FLASH_ENC_BINARY), "binary") == 0);
    assert(strcmp(flash_encode_format_name(FLASH_ENC_BASE64), "base64") == 0);
    printf("PASS: test_format_name\n");
}

static void test_null_input(void) {
    FlashEncodeResult enc;
    memset(&enc, 0, sizeof(enc));
    int rc = flash_encode_region(NULL, NULL, 0, FLASH_ENC_HEX, &enc);
    assert(rc == -1);
    printf("PASS: test_null_input\n");
}

int main(void) {
    test_hex_encode_decode();
    test_base64_encode_decode();
    test_binary_encode();
    test_format_name();
    test_null_input();
    printf("All flashencode tests passed.\n");
    return 0;
}
