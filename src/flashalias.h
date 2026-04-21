/*
 * flashalias.h - Flash region alias management interface
 */

#ifndef FLASHALIAS_H
#define FLASHALIAS_H

#include <stdio.h>

#define FLASHALIAS_MAX_ENTRIES  64
#define FLASHALIAS_NAME_LEN     64

#define FLASHALIAS_OK           0
#define FLASHALIAS_ERR_NULL    -1
#define FLASHALIAS_ERR_FULL    -2
#define FLASHALIAS_ERR_DUPLICATE -3
#define FLASHALIAS_ERR_NOT_FOUND -4

typedef struct {
    char original[FLASHALIAS_NAME_LEN];
    char alias[FLASHALIAS_NAME_LEN];
} FlashAliasEntry;

typedef struct {
    FlashAliasEntry entries[FLASHALIAS_MAX_ENTRIES];
    int count;
} FlashAliasMap;

/*
 * Initialize an alias map to an empty state.
 */
void flashalias_init(FlashAliasMap *map);

/*
 * Add an alias mapping: alias -> original.
 * Returns FLASHALIAS_OK on success, or an error code.
 */
int flashalias_add(FlashAliasMap *map, const char *original, const char *alias);

/*
 * Resolve a name: if it is a known alias, return the original name.
 * Otherwise return the name itself.
 */
const char *flashalias_resolve(const FlashAliasMap *map, const char *name);

/*
 * Remove an alias entry by alias name.
 * Returns FLASHALIAS_OK on success, or FLASHALIAS_ERR_NOT_FOUND.
 */
int flashalias_remove(FlashAliasMap *map, const char *alias);

/*
 * Return the number of registered aliases.
 */
int flashalias_count(const FlashAliasMap *map);

/*
 * Print all alias mappings to the given file stream.
 */
void flashalias_print(const FlashAliasMap *map, FILE *out);

#endif /* FLASHALIAS_H */
