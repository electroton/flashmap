/**
 * flashalias.h - Region aliasing support for flashmap
 *
 * Allows assigning one or more human-readable alias names to flash
 * regions, enabling lookups by alternate identifiers.
 */

#ifndef FLASHALIAS_H
#define FLASHALIAS_H

#include "flashregion.h"
#include <stddef.h>

#define FLASHALIAS_MAX_ALIASES 8
#define FLASHALIAS_NAME_LEN    64

/**
 * Represents a set of aliases bound to a single flash region name.
 */
typedef struct {
    char region_name[FLASHALIAS_NAME_LEN];
    char aliases[FLASHALIAS_MAX_ALIASES][FLASHALIAS_NAME_LEN];
    size_t alias_count;
} FlashAliasEntry;

/**
 * Alias table holding all alias entries.
 */
typedef struct {
    FlashAliasEntry *entries;
    size_t count;
    size_t capacity;
} FlashAliasTable;

/** Initialize an alias table. */
void flashalias_init(FlashAliasTable *table);

/** Free resources held by an alias table. */
void flashalias_free(FlashAliasTable *table);

/**
 * Register an alias for a region.
 * Returns 0 on success, -1 if the alias already exists or limits are exceeded.
 */
int flashalias_add(FlashAliasTable *table, const char *region_name,
                   const char *alias);

/**
 * Look up the canonical region name for a given alias (or region name).
 * Returns a pointer to the region name string, or NULL if not found.
 */
const char *flashalias_resolve(const FlashAliasTable *table,
                               const char *alias);

/**
 * Remove all aliases associated with a region.
 * Returns the number of entries removed.
 */
int flashalias_remove(FlashAliasTable *table, const char *region_name);

/**
 * List all aliases for a region into out_aliases (array of FLASHALIAS_NAME_LEN
 * strings). Returns the number of aliases written.
 */
size_t flashalias_list(const FlashAliasTable *table, const char *region_name,
                       char out_aliases[][FLASHALIAS_NAME_LEN],
                       size_t max_out);

#endif /* FLASHALIAS_H */
