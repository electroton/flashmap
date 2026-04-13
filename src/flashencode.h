#ifndef FLASHENCODE_H
#define FLASHENCODE_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/**
 * flashencode — Encode flash region data into portable text formats.
 * Supports Base64 and hexadecimal output for region serialization.
 */

typedef enum {
    FLASH_ENCODE_OK          =  0,
    FLASH_ENCODE_ERR_NULL    = -1,
    FLASH_ENCODE_ERR_BUFFER  = -2,
    FLASH_ENCODE_ERR_FORMAT  = -3
} FlashEncodeResult;

typedef enum {
    FLASH_ENCODE_FMT_BASE64 = 0,
    FLASH_ENCODE_FMT_HEX    = 1
} FlashEncodeFormat;

/**
 * Encode raw bytes as Base64 into out (null-terminated).
 * Returns FLASH_ENCODE_OK on success.
 */
FlashEncodeResult flash_encode_base64(const uint8_t *data, size_t len,
                                       char *out, size_t out_size);

/**
 * Encode raw bytes as lowercase hex into out (null-terminated).
 * Returns FLASH_ENCODE_OK on success.
 */
FlashEncodeResult flash_encode_hex(const uint8_t *data, size_t len,
                                    char *out, size_t out_size);

/**
 * Encode a FlashRegion descriptor (name:start:size) using the given format.
 * Returns FLASH_ENCODE_OK on success.
 */
FlashEncodeResult flash_encode_region(const FlashRegion *region,
                                       FlashEncodeFormat fmt,
                                       char *out, size_t out_size);

/** Return a human-readable string for a FlashEncodeResult code. */
const char *flash_encode_result_str(FlashEncodeResult result);

#endif /* FLASHENCODE_H */
