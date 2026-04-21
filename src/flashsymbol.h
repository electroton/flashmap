#ifndef FLASHSYMBOL_H
#define FLASHSYMBOL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_SYMBOL_NAME_MAX 128
#define FLASH_SYMBOL_MAX      256

typedef enum {
    FLASH_SYMBOL_UNKNOWN  = 0,
    FLASH_SYMBOL_FUNCTION = 1,
    FLASH_SYMBOL_OBJECT   = 2,
    FLASH_SYMBOL_SECTION  = 3
} FlashSymbolType;

typedef struct {
    char            name[FLASH_SYMBOL_NAME_MAX];
    uint32_t        address;
    uint32_t        size;
    FlashSymbolType type;
} FlashSymbol;

typedef struct {
    FlashSymbol entries[FLASH_SYMBOL_MAX];
    size_t      count;
} FlashSymbolTable;

/* Initialize an empty symbol table */
void flash_symbol_table_init(FlashSymbolTable *table);

/* Add a symbol to the table; returns 0 on success, -1 if full or invalid */
int flash_symbol_add(FlashSymbolTable *table, const char *name,
                     uint32_t address, uint32_t size, FlashSymbolType type);

/* Look up a symbol by name; returns pointer or NULL if not found */
const FlashSymbol *flash_symbol_find_by_name(const FlashSymbolTable *table,
                                             const char *name);

/* Look up a symbol by address (exact match); returns pointer or NULL */
const FlashSymbol *flash_symbol_find_by_address(const FlashSymbolTable *table,
                                                uint32_t address);

/* Return number of symbols in the table */
size_t flash_symbol_count(const FlashSymbolTable *table);

/* Clear all symbols from the table */
void flash_symbol_table_clear(FlashSymbolTable *table);

#ifdef __cplusplus
}
#endif

#endif /* FLASHSYMBOL_H */
