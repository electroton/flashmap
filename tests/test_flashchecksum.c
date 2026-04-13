#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashchecksum.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r = {0};
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start_address = start;
    r.size = size;
    return r;
}

static void test_algo_names(void) {
    assert(strcmp(flashchecksum_algo_name(CHECKSUM_CRC32),   "CRC32")    == 0);
    assert(strcmp(flashchecksum_algo_name(CHECKSUM_ADLER32), "Adler-32") == 0);
    assert(strcmp(flashchecksum_algo_name(CHECKSUM_XOR8),    "XOR8")     == 0);
    assert(strcmp(flashchecksum_algo_name(CHECKSUM_SUM32),   "SUM32")    == 0);
    printf("PASS: test_algo_names\n");
}

static void test_compute_crc32(void) {
    FlashRegion r = make_region("text", 0x08000000, 256);
    FlashChecksum cs = flashchecksum_compute(&r, CHECKSUM_CRC32);
    assert(strcmp(cs.region_name, "text") == 0);
    assert(cs.start_address == 0x08000000);
    assert(cs.size == 256);
    assert(cs.algorithm == CHECKSUM_CRC32);
    /* Value must be deterministic — compute again and compare */
    FlashChecksum cs2 = flashchecksum_compute(&r, CHECKSUM_CRC32);
    assert(cs.value == cs2.value);
    printf("PASS: test_compute_crc32\n");
}

static void test_compute_adler32(void) {
    FlashRegion r = make_region("data", 0x20000000, 128);
    FlashChecksum cs = flashchecksum_compute(&r, CHECKSUM_ADLER32);
    assert(cs.value != 0);
    FlashChecksum cs2 = flashchecksum_compute(&r, CHECKSUM_ADLER32);
    assert(cs.value == cs2.value);
    printf("PASS: test_compute_adler32\n");
}

static void test_compute_xor8_and_sum32(void) {
    FlashRegion r = make_region("bss", 0x20001000, 64);
    FlashChecksum xcs = flashchecksum_compute(&r, CHECKSUM_XOR8);
    FlashChecksum scs = flashchecksum_compute(&r, CHECKSUM_SUM32);
    assert(xcs.value <= 0xFF);
    assert(scs.value >= xcs.value);
    printf("PASS: test_compute_xor8_and_sum32\n");
}

static void test_different_regions_differ(void) {
    FlashRegion r1 = make_region("r1", 0x08000000, 512);
    FlashRegion r2 = make_region("r2", 0x08010000, 512);
    FlashChecksum cs1 = flashchecksum_compute(&r1, CHECKSUM_CRC32);
    FlashChecksum cs2 = flashchecksum_compute(&r2, CHECKSUM_CRC32);
    assert(cs1.value != cs2.value);
    printf("PASS: test_different_regions_differ\n");
}

static void test_table_add_and_verify(void) {
    FlashChecksumTable table;
    flashchecksum_table_init(&table);

    FlashRegion r = make_region("flash", 0x08000000, 1024);
    FlashChecksum cs = flashchecksum_compute(&r, CHECKSUM_CRC32);
    int added = flashchecksum_table_add(&table, cs);
    assert(added == 1);
    assert(table.count == 1);

    int ok = flashchecksum_verify(&table.entries[0], &r);
    assert(ok == 1);

    flashchecksum_table_free(&table);
    printf("PASS: test_table_add_and_verify\n");
}

static void test_verify_mismatch(void) {
    FlashRegion r1 = make_region("a", 0x08000000, 256);
    FlashRegion r2 = make_region("b", 0x08001000, 256);
    FlashChecksum cs = flashchecksum_compute(&r1, CHECKSUM_SUM32);
    int ok = flashchecksum_verify(&cs, &r2);
    assert(ok == 0);
    printf("PASS: test_verify_mismatch\n");
}

static void test_null_safety(void) {
    FlashChecksum cs = flashchecksum_compute(NULL, CHECKSUM_CRC32);
    assert(cs.value == 0);
    assert(flashchecksum_verify(NULL, NULL) == 0);
    flashchecksum_table_init(NULL);
    flashchecksum_table_free(NULL);
    printf("PASS: test_null_safety\n");
}

int main(void) {
    printf("=== flashchecksum tests ===\n");
    test_algo_names();
    test_compute_crc32();
    test_compute_adler32();
    test_compute_xor8_and_sum32();
    test_different_regions_differ();
    test_table_add_and_verify();
    test_verify_mismatch();
    test_null_safety();
    printf("All flashchecksum tests passed.\n");
    return 0;
}
