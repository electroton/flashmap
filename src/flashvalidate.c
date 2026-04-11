#include "flashvalidate.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void record_error(FlashValidateResult *result, FlashValidateCode code,
                         const char *region_name, const char *message) {
    if (result->error_count >= FLASH_VALIDATE_MAX_ERRORS) return;
    FlashValidateError *err = &result->errors[result->error_count++];
    err->code = code;
    strncpy(err->region_name, region_name ? region_name : "<unknown>",
            FLASH_REGION_NAME_MAX - 1);
    err->region_name[FLASH_REGION_NAME_MAX - 1] = '\0';
    strncpy(err->message, message, FLASH_VALIDATE_MSG_MAX - 1);
    err->message[FLASH_VALIDATE_MSG_MAX - 1] = '\0';
}

FlashValidateResult flash_validate_layout(const FlashLayout *layout) {
    FlashValidateResult result;
    memset(&result, 0, sizeof(result));
    result.valid = true;

    if (!layout || layout->region_count == 0) {
        record_error(&result, FLASH_VALIDATE_EMPTY_LAYOUT, NULL,
                     "Layout is NULL or contains no regions");
        result.valid = false;
        return result;
    }

    for (size_t i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];

        /* Check for zero-size regions */
        if (r->size == 0) {
            char msg[FLASH_VALIDATE_MSG_MAX];
            snprintf(msg, sizeof(msg), "Region '%s' has zero size", r->name);
            record_error(&result, FLASH_VALIDATE_ZERO_SIZE, r->name, msg);
            result.valid = false;
        }

        /* Check for overflow: start + size must not wrap around */
        if ((uint64_t)r->start + (uint64_t)r->size > UINT32_MAX + 1ULL) {
            char msg[FLASH_VALIDATE_MSG_MAX];
            snprintf(msg, sizeof(msg),
                     "Region '%s' overflows address space (0x%08X + 0x%08X)",
                     r->name, r->start, r->size);
            record_error(&result, FLASH_VALIDATE_OVERFLOW, r->name, msg);
            result.valid = false;
        }

        /* Check for overlaps with subsequent regions */
        for (size_t j = i + 1; j < layout->region_count; j++) {
            const FlashRegion *s = &layout->regions[j];
            uint32_t r_end = r->start + r->size;
            uint32_t s_end = s->start + s->size;
            if (r->start < s_end && s->start < r_end) {
                char msg[FLASH_VALIDATE_MSG_MAX];
                snprintf(msg, sizeof(msg),
                         "Region '%s' [0x%08X-0x%08X) overlaps '%s' [0x%08X-0x%08X)",
                         r->name, r->start, r_end,
                         s->name, s->start, s_end);
                record_error(&result, FLASH_VALIDATE_OVERLAP, r->name, msg);
                result.valid = false;
            }
        }
    }

    return result;
}

void flash_validate_print_errors(const FlashValidateResult *result, FILE *out) {
    if (!result || !out) return;
    if (result->error_count == 0) {
        fprintf(out, "Validation passed: no errors found.\n");
        return;
    }
    fprintf(out, "Validation failed with %zu error(s):\n", result->error_count);
    for (size_t i = 0; i < result->error_count; i++) {
        const FlashValidateError *e = &result->errors[i];
        fprintf(out, "  [%d] %s\n", e->code, e->message);
    }
}
