#include "flashremap.h"
#include <string.h>
#include <stdio.h>

void flashremap_init(FlashRemapTable *table)
{
    if (!table) return;
    memset(table, 0, sizeof(*table));
}

int flashremap_add(FlashRemapTable *table, const char *name,
                   uint32_t src_base, uint32_t dst_base, uint32_t size)
{
    if (!table || !name || size == 0)
        return -1;
    if (table->count >= FLASHREMAP_MAX_ENTRIES)
        return -1;

    FlashRemapEntry *e = &table->entries[table->count];
    strncpy(e->name, name, FLASHREMAP_NAME_MAX - 1);
    e->name[FLASHREMAP_NAME_MAX - 1] = '\0';
    e->src_base = src_base;
    e->dst_base = dst_base;
    e->size     = size;
    table->count++;
    return 0;
}

uint32_t flashremap_translate(const FlashRemapTable *table, uint32_t src_addr)
{
    if (!table) return src_addr;

    for (size_t i = 0; i < table->count; i++) {
        const FlashRemapEntry *e = &table->entries[i];
        if (src_addr >= e->src_base && src_addr < e->src_base + e->size) {
            uint32_t offset = src_addr - e->src_base;
            return e->dst_base + offset;
        }
    }
    return src_addr;
}

int flashremap_apply(const FlashRemapTable *table, FlashRegion *region)
{
    if (!table || !region) return 0;

    for (size_t i = 0; i < table->count; i++) {
        const FlashRemapEntry *e = &table->entries[i];
        if (region->address >= e->src_base &&
            region->address < e->src_base + e->size) {
            uint32_t offset  = region->address - e->src_base;
            region->address  = e->dst_base + offset;
            return 1;
        }
    }
    return 0;
}

const FlashRemapEntry *flashremap_find(const FlashRemapTable *table,
                                       const char *name)
{
    if (!table || !name) return NULL;

    for (size_t i = 0; i < table->count; i++) {
        if (strncmp(table->entries[i].name, name, FLASHREMAP_NAME_MAX) == 0)
            return &table->entries[i];
    }
    return NULL;
}

void flashremap_clear(FlashRemapTable *table)
{
    if (!table) return;
    memset(table, 0, sizeof(*table));
}
