#include "flashsymbol.h"
#include <string.h>
#include <stdio.h>

void flash_symbol_table_init(FlashSymbolTable *table)
{
    if (!table) return;
    memset(table, 0, sizeof(*table));
}

int flash_symbol_add(FlashSymbolTable *table, const char *name,
                     uint32_t address, uint32_t size, FlashSymbolType type)
{
    if (!table || !name || name[0] == '\0') return -1;
    if (table->count >= FLASH_SYMBOL_MAX) return -1;

    FlashSymbol *sym = &table->entries[table->count];
    strncpy(sym->name, name, FLASH_SYMBOL_NAME_MAX - 1);
    sym->name[FLASH_SYMBOL_NAME_MAX - 1] = '\0';
    sym->address = address;
    sym->size    = size;
    sym->type    = type;
    table->count++;
    return 0;
}

const FlashSymbol *flash_symbol_find_by_name(const FlashSymbolTable *table,
                                             const char *name)
{
    if (!table || !name) return NULL;
    for (size_t i = 0; i < table->count; i++) {
        if (strncmp(table->entries[i].name, name, FLASH_SYMBOL_NAME_MAX) == 0)
            return &table->entries[i];
    }
    return NULL;
}

const FlashSymbol *flash_symbol_find_by_address(const FlashSymbolTable *table,
                                                uint32_t address)
{
    if (!table) return NULL;
    for (size_t i = 0; i < table->count; i++) {
        if (table->entries[i].address == address)
            return &table->entries[i];
    }
    return NULL;
}

size_t flash_symbol_count(const FlashSymbolTable *table)
{
    if (!table) return 0;
    return table->count;
}

void flash_symbol_table_clear(FlashSymbolTable *table)
{
    if (!table) return;
    memset(table, 0, sizeof(*table));
}
