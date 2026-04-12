#ifndef FLASHPATCH_H
#define FLASHPATCH_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"
#include "flashlayout.h"

/* Patch operation types */
typedef enum {
    PATCH_OP_RENAME,
    PATCH_OP_RESIZE,
    PATCH_OP_REBASE,
    PATCH_OP_ADD,
    PATCH_OP_REMOVE
} FlashPatchOpType;

/* A single patch operation targeting a named region */
typedef struct {
    FlashPatchOpType op;
    char            target[64];   /* region name to patch */
    char            new_name[64]; /* used by PATCH_OP_RENAME */
    uint32_t        new_address;  /* used by PATCH_OP_REBASE */
    uint32_t        new_size;     /* used by PATCH_OP_RESIZE */
    FlashRegion     new_region;   /* used by PATCH_OP_ADD   */
} FlashPatchOp;

/* A patch set: ordered list of operations */
typedef struct {
    FlashPatchOp *ops;
    size_t        count;
    size_t        capacity;
} FlashPatchSet;

/* Lifecycle */
FlashPatchSet *flashpatch_create(void);
void           flashpatch_destroy(FlashPatchSet *ps);

/* Building a patch set */
int flashpatch_add_rename(FlashPatchSet *ps, const char *target, const char *new_name);
int flashpatch_add_resize(FlashPatchSet *ps, const char *target, uint32_t new_size);
int flashpatch_add_rebase(FlashPatchSet *ps, const char *target, uint32_t new_address);
int flashpatch_add_region(FlashPatchSet *ps, const FlashRegion *region);
int flashpatch_remove_region(FlashPatchSet *ps, const char *target);

/* Apply a patch set to a layout (modifies a copy, returns new layout) */
FlashLayout *flashpatch_apply(const FlashLayout *layout, const FlashPatchSet *ps);

/* Describe patch set to stdout */
void flashpatch_print(const FlashPatchSet *ps);

#endif /* FLASHPATCH_H */
