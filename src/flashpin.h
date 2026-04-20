#ifndef FLASHPIN_H
#define FLASHPIN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FLASH_PIN_NAME_MAX 64

/**
 * Type of a flash pin — describes its semantic role.
 */
typedef enum {
    FLASH_PIN_ENTRY    = 0,  /**< Code/data entry point */
    FLASH_PIN_BOUNDARY = 1,  /**< Region boundary marker */
    FLASH_PIN_MARKER   = 2   /**< Generic debug/analysis marker */
} FlashPinType;

/**
 * A single named address pin within the flash address space.
 */
typedef struct {
    char         name[FLASH_PIN_NAME_MAX];
    uint32_t     address;
    FlashPinType type;
    bool         locked;  /**< Locked pins cannot be removed */
} FlashPin;

/**
 * A dynamic collection of flash pins.
 */
typedef struct {
    FlashPin *pins;
    size_t    count;
    size_t    capacity;
} FlashPinSet;

FlashPinSet *flashpin_create(void);
void         flashpin_destroy(FlashPinSet *ps);

int       flashpin_add(FlashPinSet *ps, const char *name, uint32_t address, FlashPinType type);
int       flashpin_remove(FlashPinSet *ps, const char *name);
FlashPin *flashpin_find(FlashPinSet *ps, const char *name);
FlashPin *flashpin_find_by_address(FlashPinSet *ps, uint32_t address);
int       flashpin_lock(FlashPinSet *ps, const char *name);
int       flashpin_unlock(FlashPinSet *ps, const char *name);
size_t    flashpin_count(const FlashPinSet *ps);

#endif /* FLASHPIN_H */
