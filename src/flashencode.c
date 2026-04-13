#include "flashencode.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

FlashEncodeResult flash_encode_base64(const uint8_t *data, size_t len, char *out, size_t out_size) {
    if (!data || !out || out_size == 0) return FLASH_ENCODE_ERR_NULL;
    size_t required = 4 * ((len + 2) / 3) + 1;
    if (out_size < required) return FLASH_ENCODE_ERR_BUFFER;

    size_t i = 0, j = 0;
    while (i < len) {
        uint32_t octet_a = i < len ? data[i++] : 0;
        uint32_t octet_b = i < len ? data[i++] : 0;
        uint32_t octet_c = i < len ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;
        out[j++] = base64_table[(triple >> 18) & 0x3F];
        out[j++] = base64_table[(triple >> 12) & 0x3F];
        out[j++] = base64_table[(triple >>  6) & 0x3F];
        out[j++] = base64_table[(triple >>  0) & 0x3F];
    }
    size_t pad = (3 - len % 3) % 3;
    for (size_t p = 0; p < pad; p++) out[j - 1 - p] = '=';
    out[j] = '\0';
    return FLASH_ENCODE_OK;
}

FlashEncodeResult flash_encode_hex(const uint8_t *data, size_t len, char *out, size_t out_size) {
    if (!data || !out || out_size == 0) return FLASH_ENCODE_ERR_NULL;
    if (out_size < len * 2 + 1) return FLASH_ENCODE_ERR_BUFFER;
    for (size_t i = 0; i < len; i++) {
        snprintf(out + i * 2, 3, "%02x", data[i]);
    }
    out[len * 2] = '\0';
    return FLASH_ENCODE_OK;
}

FlashEncodeResult flash_encode_region(const FlashRegion *region, FlashEncodeFormat fmt,
                                       char *out, size_t out_size) {
    if (!region || !out) return FLASH_ENCODE_ERR_NULL;
    uint8_t buf[64];
    int n = snprintf((char *)buf, sizeof(buf), "%s:0x%08X:%u",
                     region->name, region->start, region->size);
    if (n < 0 || (size_t)n >= sizeof(buf)) return FLASH_ENCODE_ERR_BUFFER;
    if (fmt == FLASH_ENCODE_FMT_BASE64)
        return flash_encode_base64(buf, (size_t)n, out, out_size);
    if (fmt == FLASH_ENCODE_FMT_HEX)
        return flash_encode_hex(buf, (size_t)n, out, out_size);
    return FLASH_ENCODE_ERR_FORMAT;
}

const char *flash_encode_result_str(FlashEncodeResult result) {
    switch (result) {
        case FLASH_ENCODE_OK:          return "OK";
        case FLASH_ENCODE_ERR_NULL:    return "Null pointer";
        case FLASH_ENCODE_ERR_BUFFER:  return "Buffer too small";
        case FLASH_ENCODE_ERR_FORMAT:  return "Unknown format";
        default:                       return "Unknown error";
    }
}
