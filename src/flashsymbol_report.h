#ifndef FLASHSYMBOL_REPORT_H
#define FLASHSYMBOL_REPORT_H

#include "flashsymbol.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Print a formatted symbol table report to the given stream */
void flash_symbol_report_print(const FlashSymbolTable *table, FILE *out);

/* Print a single symbol entry */
void flash_symbol_report_entry(const FlashSymbol *sym, FILE *out);

#ifdef __cplusplus
}
#endif

#endif /* FLASHSYMBOL_REPORT_H */
