/*
 * flashalias.c - Flash region alias management
 *
 * Allows regions to be referenced by alternate names,
 * enabling symbolic renaming without modifying the original layout.
 */

#include "flashalias.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void flashalias_init(FlashAliasMap *map) {
    if (!map) return;
    map->count = 0;
    memset(map->entries, 0, sizeof(map->entries));
}

int flashalias_add(FlashAliasMap *map, const char *original, const char *alias) {
    if (!map || !original || !alias) return FLASHALIAS_ERR_NULL;
    if (map->count >= FLASHALIAS_MAX_ENTRIES) return FLASHALIAS_ERR_FULL;

    /* Check for duplicate alias */
    for (int i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].alias, alias) == 0)
            return FLASHALIAS_ERR_DUPLICATE;
    }

    strncpy(map->entries[map->count].original, original, FLASHALIAS_NAME_LEN - 1);
    map->entries[map->count].original[FLASHALIAS_NAME_LEN - 1] = '\0';
    strncpy(map->entries[map->count].alias, alias, FLASHALIAS_NAME_LEN - 1);
    map->entries[map->count].alias[FLASHALIAS_NAME_LEN - 1] = '\0';
    map->count++;
    return FLASHALIAS_OK;
}

const char *flashalias_resolve(const FlashAliasMap *map, const char *name) {
    if (!map || !name) return NULL;
    for (int i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].alias, name) == 0)
            return map->entries[i].original;
    }
    return name; /* Return original if no alias found */
}

int flashalias_remove(FlashAliasMap *map, const char *alias) {
    if (!map || !alias) return FLASHALIAS_ERR_NULL;
    for (int i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].alias, alias) == 0) {
            /* Shift remaining entries down */
            for (int j = i; j < map->count - 1; j++)
                map->entries[j] = map->entries[j + 1];
            map->count--;
            return FLASHALIAS_OK;
        }
    }
    return FLASHALIAS_ERR_NOT_FOUND;
}

int flashalias_count(const FlashAliasMap *map) {
    if (!map) return 0;
    return map->count;
}

void flashalias_print(const FlashAliasMap *map, FILE *out) {
    if (!map || !out) return;
    fprintf(out, "Flash Alias Map (%d entries):\n", map->count);
    for (int i = 0; i < map->count; i++) {
        fprintf(out, "  [%d] '%s' -> '%s'\n",
                i, map->entries[i].alias, map->entries[i].original);
    }
}
