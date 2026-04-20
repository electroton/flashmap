#ifndef FLASHPIN_H
#define FLASHPIN_H

#include "flashregion.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * flashpin — Pin (bookmark) specific addresses or regions in a flash layout
 * for quick reference and cross-module annotation.
 */

#define FLASH_PIN_MAX_LABEL 64
#define FLASH_PIN_MAX_PINS  64

typedef enum {
    FLASH_PIN_TYPE_ADDRESS = 0,  /* Pin a specific address */
    FLASH_PIN_TYPE_REGION,       /* Pin an entire named region */
    FLASH_PIN_TYPE_OFFSET        /* Pin a byte offset within a region */
} FlashPinType;

typedef struct {
    char        label[FLASH_PIN_MAX_LABEL];
    FlashPinType type;
    uint32_t    address;         /* Resolved absolute address */
    char        region_name[FLASH_PIN_MAX_LABEL];
    uint32_t    offset;          /* Offset within region (type OFFSET only) */
    bool        active;
} FlashPin;

typedef struct {
    FlashPin pins[FLASH_PIN_MAX_PINS];
    int      count;
} FlashPinSet;

/* Initialise an empty pin set */
void flash_pin_set_init(FlashPinSet *ps);

/* Add a pin by absolute address */
bool flash_pin_add_address(FlashPinSet *ps, const char *label, uint32_t address);

/* Add a pin referencing a named region */
bool flash_pin_add_region(FlashPinSet *ps, const char *label,
                          const FlashRegion *region);

/* Add a pin at an offset within a named region */
bool flash_pin_add_offset(FlashPinSet *ps, const char *label,
                          const FlashRegion *region, uint32_t offset);

/* Remove a pin by label; returns true if found and removed */
bool flash_pin_remove(FlashPinSet *ps, const char *label);

/* Look up a pin by label; returns NULL if not found */
const FlashPin *flash_pin_find(const FlashPinSet *ps, const char *label);

/* Check whether an address falls on any active pin */
bool flash_pin_hits_address(const FlashPinSet *ps, uint32_t address,
                            const FlashPin **out_pin);

/* Print all pins to stdout */
void flash_pin_dump(const FlashPinSet *ps);

#endif /* FLASHPIN_H */
