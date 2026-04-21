#ifndef FLASHRESOLVE_H
#define FLASHRESOLVE_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"
#include "flashlayout.h"

/*
 * flashresolve — Resolve symbolic address references within flash regions.
 *
 * Given a layout and a symbolic name or address expression, resolve it
 * to a concrete physical address within the flash map.
 */

typedef enum {
    RESOLVE_OK = 0,
    RESOLVE_NOT_FOUND,
    RESOLVE_AMBIGUOUS,
    RESOLVE_OUT_OF_RANGE,
    RESOLVE_INVALID_EXPR
} FlashResolveStatus;

typedef struct {
    const char *symbol;       /* Original symbol or expression string */
    uint32_t    address;      /* Resolved physical address */
    uint32_t    offset;       /* Offset within the containing region */
    const char *region_name;  /* Name of the region containing the address */
    FlashResolveStatus status;
} FlashResolveResult;

/*
 * Resolve a symbol name to its address using the provided layout.
 * Returns RESOLVE_OK and populates result on success.
 */
FlashResolveStatus flash_resolve_symbol(const FlashLayout *layout,
                                        const char *symbol,
                                        FlashResolveResult *result);

/*
 * Resolve a raw address to its containing region and offset.
 * Returns RESOLVE_OK and populates result on success.
 */
FlashResolveStatus flash_resolve_address(const FlashLayout *layout,
                                         uint32_t address,
                                         FlashResolveResult *result);

/*
 * Resolve an expression of the form "REGION+offset" or "0xADDR".
 * Returns RESOLVE_OK and populates result on success.
 */
FlashResolveStatus flash_resolve_expr(const FlashLayout *layout,
                                      const char *expr,
                                      FlashResolveResult *result);

/* Return a human-readable string for a resolve status code. */
const char *flash_resolve_status_str(FlashResolveStatus status);

#endif /* FLASHRESOLVE_H */
