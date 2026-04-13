/**
 * flashchecksum.h - Flash region checksum computation and verification
 */

#ifndef FLASHCHECKSUM_H
#define FLASHCHECKSUM_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

typedef enum {
    CHECKSUM_CRC32,
    CHECKSUM_ADLER32,
    CHECKSUM_XOR8,
    CHECKSUM_SUM32
} ChecksumAlgorithm;

typedef struct {
    char region_name[64];
    uint32_t start_address;
    uint32_t size;
    uint32_t value;
    ChecksumAlgorithm algorithm;
} FlashChecksum;

typedef struct {
    FlashChecksum *entries;
    size_t count;
    size_t capacity;
} FlashChecksumTable;

/**
 * Initialize a checksum table.
 */
void flashchecksum_table_init(FlashChecksumTable *table);

/**
 * Free resources held by a checksum table.
 */
void flashchecksum_table_free(FlashChecksumTable *table);

/**
 * Compute a checksum for a flash region using the given algorithm.
 * The region's size and start_address are used; data is simulated via address.
 */
FlashChecksum flashchecksum_compute(const FlashRegion *region, ChecksumAlgorithm algo);

/**
 * Add a computed checksum entry to the table.
 */
int flashchecksum_table_add(FlashChecksumTable *table, FlashChecksum entry);

/**
 * Verify a checksum entry against a recomputed value.
 * Returns 1 if match, 0 if mismatch.
 */
int flashchecksum_verify(const FlashChecksum *entry, const FlashRegion *region);

/**
 * Return string name of algorithm.
 */
const char *flashchecksum_algo_name(ChecksumAlgorithm algo);

#endif /* FLASHCHECKSUM_H */
