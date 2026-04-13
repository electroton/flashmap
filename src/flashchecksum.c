#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flashchecksum.h"

#define INITIAL_CAPACITY 8

void flashchecksum_table_init(FlashChecksumTable *table) {
    if (!table) return;
    table->entries = malloc(INITIAL_CAPACITY * sizeof(FlashChecksum));
    table->count = 0;
    table->capacity = table->entries ? INITIAL_CAPACITY : 0;
}

void flashchecksum_table_free(FlashChecksumTable *table) {
    if (!table) return;
    free(table->entries);
    table->entries = NULL;
    table->count = 0;
    table->capacity = 0;
}

static uint32_t compute_crc32(uint32_t start, uint32_t size) {
    /* Deterministic simulation using address and size */
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < size; i++) {
        uint8_t byte = (uint8_t)((start + i) & 0xFF);
        crc ^= byte;
        for (int b = 0; b < 8; b++) {
            if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
            else crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

static uint32_t compute_adler32(uint32_t start, uint32_t size) {
    uint32_t a = 1, b = 0;
    for (uint32_t i = 0; i < size; i++) {
        uint8_t byte = (uint8_t)((start + i) & 0xFF);
        a = (a + byte) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

static uint32_t compute_xor8(uint32_t start, uint32_t size) {
    uint8_t acc = 0;
    for (uint32_t i = 0; i < size; i++) {
        acc ^= (uint8_t)((start + i) & 0xFF);
    }
    return acc;
}

static uint32_t compute_sum32(uint32_t start, uint32_t size) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < size; i++) {
        sum += (uint8_t)((start + i) & 0xFF);
    }
    return sum;
}

FlashChecksum flashchecksum_compute(const FlashRegion *region, ChecksumAlgorithm algo) {
    FlashChecksum cs = {0};
    if (!region) return cs;

    strncpy(cs.region_name, region->name, sizeof(cs.region_name) - 1);
    cs.start_address = region->start_address;
    cs.size = region->size;
    cs.algorithm = algo;

    switch (algo) {
        case CHECKSUM_CRC32:   cs.value = compute_crc32(region->start_address, region->size); break;
        case CHECKSUM_ADLER32: cs.value = compute_adler32(region->start_address, region->size); break;
        case CHECKSUM_XOR8:    cs.value = compute_xor8(region->start_address, region->size); break;
        case CHECKSUM_SUM32:   cs.value = compute_sum32(region->start_address, region->size); break;
    }
    return cs;
}

int flashchecksum_table_add(FlashChecksumTable *table, FlashChecksum entry) {
    if (!table) return 0;
    if (table->count >= table->capacity) {
        size_t new_cap = table->capacity * 2;
        FlashChecksum *tmp = realloc(table->entries, new_cap * sizeof(FlashChecksum));
        if (!tmp) return 0;
        table->entries = tmp;
        table->capacity = new_cap;
    }
    table->entries[table->count++] = entry;
    return 1;
}

int flashchecksum_verify(const FlashChecksum *entry, const FlashRegion *region) {
    if (!entry || !region) return 0;
    FlashChecksum recomputed = flashchecksum_compute(region, entry->algorithm);
    return recomputed.value == entry->value;
}

const char *flashchecksum_algo_name(ChecksumAlgorithm algo) {
    switch (algo) {
        case CHECKSUM_CRC32:   return "CRC32";
        case CHECKSUM_ADLER32: return "Adler-32";
        case CHECKSUM_XOR8:    return "XOR8";
        case CHECKSUM_SUM32:   return "SUM32";
        default:               return "Unknown";
    }
}
