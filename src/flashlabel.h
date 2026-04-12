#ifndef FLASHLABEL_H
#define FLASHLABEL_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASHLABEL_MAX_NAME   64
#define FLASHLABEL_MAX_NOTE   128
#define FLASHLABEL_MAX_LABELS 64

typedef enum {
    FLASHLABEL_KIND_SECTION = 0,
    FLASHLABEL_KIND_SYMBOL,
    FLASHLABEL_KIND_BOUNDARY,
    FLASHLABEL_KIND_CUSTOM
} FlashLabelKind;

typedef struct {
    char         name[FLASHLABEL_MAX_NAME];
    char         note[FLASHLABEL_MAX_NOTE];
    uint32_t     address;
    uint32_t     size;
    FlashLabelKind kind;
    int          pinned;   /* 1 = label survives layout changes */
} FlashLabel;

typedef struct {
    FlashLabel entries[FLASHLABEL_MAX_LABELS];
    size_t     count;
} FlashLabelSet;

/* Lifecycle */
void flashlabel_set_init(FlashLabelSet *set);

/* CRUD */
int  flashlabel_add(FlashLabelSet *set, const char *name, uint32_t address,
                    uint32_t size, FlashLabelKind kind, const char *note);
int  flashlabel_remove(FlashLabelSet *set, const char *name);
const FlashLabel *flashlabel_find(const FlashLabelSet *set, const char *name);
const FlashLabel *flashlabel_at(const FlashLabelSet *set, uint32_t address);

/* Bulk helpers */
int  flashlabel_from_region(FlashLabelSet *set, const FlashRegion *region,
                             FlashLabelKind kind);
size_t flashlabel_overlapping(const FlashLabelSet *set, uint32_t addr,
                               uint32_t size, FlashLabel *out, size_t out_max);

/* Pin control */
void flashlabel_pin(FlashLabelSet *set, const char *name);
void flashlabel_unpin(FlashLabelSet *set, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* FLASHLABEL_H */
