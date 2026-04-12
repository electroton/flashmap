/**
 * flashscope.h - Flash region scope and visibility management
 *
 * Provides mechanisms to define named scopes over flash regions,
 * enabling selective inspection and filtering by scope boundaries.
 */

#ifndef FLASHSCOPE_H
#define FLASHSCOPE_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>
#include <stdbool.h>

#define FLASHSCOPE_MAX_NAME   64
#define FLASHSCOPE_MAX_SCOPES 32

typedef struct {
    char     name[FLASHSCOPE_MAX_NAME];
    uint32_t base_addr;
    uint32_t size;
    bool     active;
} FlashScope;

typedef struct {
    FlashScope scopes[FLASHSCOPE_MAX_SCOPES];
    int        count;
} FlashScopeSet;

/**
 * Initialize an empty scope set.
 */
void flashscope_init(FlashScopeSet *set);

/**
 * Add a named scope covering [base_addr, base_addr+size).
 * Returns 0 on success, -1 if the set is full or name is NULL.
 */
int flashscope_add(FlashScopeSet *set, const char *name,
                   uint32_t base_addr, uint32_t size);

/**
 * Find a scope by name. Returns pointer to scope or NULL if not found.
 */
FlashScope *flashscope_find(FlashScopeSet *set, const char *name);

/**
 * Check whether a given address falls within any active scope.
 */
bool flashscope_contains(const FlashScopeSet *set, uint32_t addr);

/**
 * Filter a layout, returning only regions that fall within active scopes.
 * Output regions are written to out_layout (must be pre-allocated).
 * Returns the number of regions written.
 */
int flashscope_filter_layout(const FlashScopeSet *set,
                             const FlashLayout *layout,
                             FlashLayout *out_layout);

/**
 * Enable or disable a scope by name.
 * Returns 0 on success, -1 if not found.
 */
int flashscope_set_active(FlashScopeSet *set, const char *name, bool active);

#endif /* FLASHSCOPE_H */
