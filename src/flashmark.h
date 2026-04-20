#ifndef FLASHMARK_H
#define FLASHMARK_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/* Maximum number of marks per layout */
#define FLASHMARK_MAX 64
#define FLASHMARK_NAME_LEN 48

typedef enum {
    FLASHMARK_TYPE_START  = 0,
    FLASHMARK_TYPE_END    = 1,
    FLASHMARK_TYPE_CUSTOM = 2
} FlashMarkType;

typedef struct {
    char        name[FLASHMARK_NAME_LEN];
    uint32_t    address;
    FlashMarkType type;
    int         region_index; /* -1 if not bound to a region */
} FlashMark;

typedef struct {
    FlashMark   marks[FLASHMARK_MAX];
    size_t      count;
} FlashMarkSet;

/* Initialise an empty mark set */
void flashmark_init(FlashMarkSet *set);

/* Add a mark; returns 0 on success, -1 if full or invalid */
int flashmark_add(FlashMarkSet *set, const char *name, uint32_t address,
                  FlashMarkType type, int region_index);

/* Find a mark by name; returns pointer or NULL */
const FlashMark *flashmark_find(const FlashMarkSet *set, const char *name);

/* Remove a mark by name; returns 0 on success, -1 if not found */
int flashmark_remove(FlashMarkSet *set, const char *name);

/* Return all marks whose address falls within [start, start+size) */
size_t flashmark_in_range(const FlashMarkSet *set, uint32_t start,
                          uint32_t size, const FlashMark **out, size_t out_max);

/* Sort marks by address (ascending) */
void flashmark_sort(FlashMarkSet *set);

#endif /* FLASHMARK_H */
