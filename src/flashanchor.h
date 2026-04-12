#ifndef FLASHANCHOR_H
#define FLASHANCHOR_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/*
 * flashanchor: Fixed reference points in flash memory layout.
 * Anchors mark absolute addresses that must remain stable across builds,
 * such as bootloader entry points, interrupt vector tables, or config pages.
 */

#define FLASH_ANCHOR_NAME_MAX 64
#define FLASH_ANCHOR_MAX      32

typedef enum {
    ANCHOR_TYPE_ENTRY    = 0,  /* Entry point (e.g. reset vector) */
    ANCHOR_TYPE_VECTOR   = 1,  /* Interrupt vector table */
    ANCHOR_TYPE_CONFIG   = 2,  /* Configuration/NVM page */
    ANCHOR_TYPE_BOUNDARY = 3,  /* Region boundary marker */
    ANCHOR_TYPE_CUSTOM   = 4
} FlashAnchorType;

typedef struct {
    char           name[FLASH_ANCHOR_NAME_MAX];
    uint32_t       address;
    uint32_t       size;       /* 0 means point anchor, >0 means range anchor */
    FlashAnchorType type;
    int            required;   /* Non-zero if violation should be an error */
} FlashAnchor;

typedef struct {
    FlashAnchor entries[FLASH_ANCHOR_MAX];
    size_t      count;
} FlashAnchorSet;

/* Initialize an empty anchor set */
void flash_anchor_set_init(FlashAnchorSet *set);

/* Add an anchor; returns 0 on success, -1 if full or invalid */
int flash_anchor_add(FlashAnchorSet *set, const char *name,
                     uint32_t address, uint32_t size,
                     FlashAnchorType type, int required);

/* Check whether a FlashRegion satisfies all anchors in the set.
 * Returns the number of violations found. */
int flash_anchor_validate(const FlashAnchorSet *set,
                          const FlashRegion *regions, size_t count,
                          char *report_buf, size_t report_buf_len);

/* Find an anchor by name; returns pointer or NULL */
const FlashAnchor *flash_anchor_find(const FlashAnchorSet *set,
                                     const char *name);

/* Print all anchors to stdout */
void flash_anchor_print(const FlashAnchorSet *set);

const char *flash_anchor_type_str(FlashAnchorType type);

#endif /* FLASHANCHOR_H */
