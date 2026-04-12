#include "flashindex.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashIndex *flash_index_create(void) {
    FlashIndex *idx = calloc(1, sizeof(FlashIndex));
    if (!idx) return NULL;
    idx->capacity = 16;
    idx->entries = calloc(idx->capacity, sizeof(FlashIndexEntry));
    if (!idx->entries) {
        free(idx);
        return NULL;
    }
    idx->count = 0;
    return idx;
}

void flash_index_destroy(FlashIndex *idx) {
    if (!idx) return;
    for (size_t i = 0; i < idx->count; i++) {
        free(idx->entries[i].name);
    }
    free(idx->entries);
    free(idx);
}

int flash_index_insert(FlashIndex *idx, const char *name, uint32_t address, uint32_t size) {
    if (!idx || !name) return -1;
    if (idx->count >= idx->capacity) {
        size_t new_cap = idx->capacity * 2;
        FlashIndexEntry *tmp = realloc(idx->entries, new_cap * sizeof(FlashIndexEntry));
        if (!tmp) return -1;
        idx->entries = tmp;
        idx->capacity = new_cap;
    }
    FlashIndexEntry *e = &idx->entries[idx->count];
    e->name = strdup(name);
    if (!e->name) return -1;
    e->address = address;
    e->size = size;
    idx->count++;
    return 0;
}

const FlashIndexEntry *flash_index_find_by_name(const FlashIndex *idx, const char *name) {
    if (!idx || !name) return NULL;
    for (size_t i = 0; i < idx->count; i++) {
        if (strcmp(idx->entries[i].name, name) == 0)
            return &idx->entries[i];
    }
    return NULL;
}

const FlashIndexEntry *flash_index_find_by_address(const FlashIndex *idx, uint32_t address) {
    if (!idx) return NULL;
    for (size_t i = 0; i < idx->count; i++) {
        uint32_t start = idx->entries[i].address;
        uint32_t end   = start + idx->entries[i].size;
        if (address >= start && address < end)
            return &idx->entries[i];
    }
    return NULL;
}

size_t flash_index_count(const FlashIndex *idx) {
    return idx ? idx->count : 0;
}

void flash_index_clear(FlashIndex *idx) {
    if (!idx) return;
    for (size_t i = 0; i < idx->count; i++)
        free(idx->entries[i].name);
    idx->count = 0;
}
