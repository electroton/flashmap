#include "flashresolve.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

const char *flash_resolve_status_str(FlashResolveStatus status) {
    switch (status) {
        case RESOLVE_OK:            return "OK";
        case RESOLVE_NOT_FOUND:     return "symbol not found";
        case RESOLVE_AMBIGUOUS:     return "ambiguous reference";
        case RESOLVE_OUT_OF_RANGE:  return "address out of range";
        case RESOLVE_INVALID_EXPR:  return "invalid expression";
        default:                    return "unknown error";
    }
}

FlashResolveStatus flash_resolve_symbol(const FlashLayout *layout,
                                        const char *symbol,
                                        FlashResolveResult *result) {
    if (!layout || !symbol || !result) return RESOLVE_INVALID_EXPR;

    int matches = 0;
    for (size_t i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (strcmp(r->name, symbol) == 0) {
            matches++;
            result->symbol      = symbol;
            result->address     = r->start;
            result->offset      = 0;
            result->region_name = r->name;
        }
    }
    if (matches == 0) { result->status = RESOLVE_NOT_FOUND;  return RESOLVE_NOT_FOUND; }
    if (matches  > 1) { result->status = RESOLVE_AMBIGUOUS;  return RESOLVE_AMBIGUOUS; }
    result->status = RESOLVE_OK;
    return RESOLVE_OK;
}

FlashResolveStatus flash_resolve_address(const FlashLayout *layout,
                                         uint32_t address,
                                         FlashResolveResult *result) {
    if (!layout || !result) return RESOLVE_INVALID_EXPR;

    for (size_t i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        if (address >= r->start && address < r->start + r->size) {
            result->symbol      = NULL;
            result->address     = address;
            result->offset      = address - r->start;
            result->region_name = r->name;
            result->status      = RESOLVE_OK;
            return RESOLVE_OK;
        }
    }
    result->status = RESOLVE_OUT_OF_RANGE;
    return RESOLVE_OUT_OF_RANGE;
}

FlashResolveStatus flash_resolve_expr(const FlashLayout *layout,
                                      const char *expr,
                                      FlashResolveResult *result) {
    if (!layout || !expr || !result) return RESOLVE_INVALID_EXPR;

    /* Try "REGION+offset" form first */
    const char *plus = strchr(expr, '+');
    if (plus) {
        char region_buf[64] = {0};
        size_t name_len = (size_t)(plus - expr);
        if (name_len >= sizeof(region_buf)) return RESOLVE_INVALID_EXPR;
        strncpy(region_buf, expr, name_len);

        char *end;
        errno = 0;
        uint32_t offset = (uint32_t)strtoul(plus + 1, &end, 0);
        if (errno || end == plus + 1) return RESOLVE_INVALID_EXPR;

        for (size_t i = 0; i < layout->count; i++) {
            const FlashRegion *r = &layout->regions[i];
            if (strcmp(r->name, region_buf) == 0) {
                if (offset >= r->size) {
                    result->status = RESOLVE_OUT_OF_RANGE;
                    return RESOLVE_OUT_OF_RANGE;
                }
                result->symbol      = expr;
                result->address     = r->start + offset;
                result->offset      = offset;
                result->region_name = r->name;
                result->status      = RESOLVE_OK;
                return RESOLVE_OK;
            }
        }
        result->status = RESOLVE_NOT_FOUND;
        return RESOLVE_NOT_FOUND;
    }

    /* Try raw numeric address */
    char *end;
    errno = 0;
    uint32_t addr = (uint32_t)strtoul(expr, &end, 0);
    if (!errno && end != expr && *end == '\0') {
        return flash_resolve_address(layout, addr, result);
    }

    /* Fall back to symbol lookup */
    return flash_resolve_symbol(layout, expr, result);
}
