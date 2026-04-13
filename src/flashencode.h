#ifndef FLASHENCODE_H
#define FLASHENCODE_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/*
 * flashencode — Encode flash region data into portable binary or hex formats.
 */

typedef enum {
    FLASH_ENC_HEX    = 0,  /* Intel HEX-style encoding */
    FLASH_ENC_BINARY = 1,  /* Raw binary encoding */
    FLASH_ENC_BASE64 = 2   /* Base64 encoding */
} FlashEncodeFormat;

typedef struct {
    FlashEncodeFormat format;
    uint8_t          *data;
    size_t            data_len;
    char             *encoded;   /* Null-terminated output string (hex/base64) */
    size_t            encoded_len;
} FlashEncodeResult;

/*
 * Encode the raw bytes of a flash region.
 * Caller must free result with flash_encode_result_free().
 * Returns 0 on success, -1 on error.
 */
int flash_encode_region(const FlashRegion *region,
                        const uint8_t     *raw_data,
                        size_t             raw_len,
                        FlashEncodeFormat  format,
                        FlashEncodeResult *out);

/*
 * Decode a previously encoded buffer back to raw bytes.
 * Caller must free out->data.
 * Returns 0 on success, -1 on error.
 */
int flash_decode_region(const char        *encoded,
                        size_t             encoded_len,
                        FlashEncodeFormat  format,
                        FlashEncodeResult *out);

/* Free internal buffers of a FlashEncodeResult. */
void flash_encode_result_free(FlashEncodeResult *result);

/* Return a human-readable name for the given format. */
const char *flash_encode_format_name(FlashEncodeFormat format);

#endif /* FLASHENCODE_H */
