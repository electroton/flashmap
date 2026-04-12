#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flashpatch.h"

#define INITIAL_CAPACITY 8

FlashPatchSet *flashpatch_create(void) {
    FlashPatchSet *ps = calloc(1, sizeof(FlashPatchSet));
    if (!ps) return NULL;
    ps->ops = malloc(INITIAL_CAPACITY * sizeof(FlashPatchOp));
    if (!ps->ops) { free(ps); return NULL; }
    ps->capacity = INITIAL_CAPACITY;
    return ps;
}

void flashpatch_destroy(FlashPatchSet *ps) {
    if (!ps) return;
    free(ps->ops);
    free(ps);
}

static int ensure_capacity(FlashPatchSet *ps) {
    if (ps->count < ps->capacity) return 0;
    size_t new_cap = ps->capacity * 2;
    FlashPatchOp *tmp = realloc(ps->ops, new_cap * sizeof(FlashPatchOp));
    if (!tmp) return -1;
    ps->ops = tmp;
    ps->capacity = new_cap;
    return 0;
}

int flashpatch_add_rename(FlashPatchSet *ps, const char *target, const char *new_name) {
    if (!ps || !target || !new_name) return -1;
    if (ensure_capacity(ps) != 0) return -1;
    FlashPatchOp *op = &ps->ops[ps->count++];
    memset(op, 0, sizeof(*op));
    op->op = PATCH_OP_RENAME;
    strncpy(op->target,   target,   sizeof(op->target)   - 1);
    strncpy(op->new_name, new_name, sizeof(op->new_name) - 1);
    return 0;
}

int flashpatch_add_resize(FlashPatchSet *ps, const char *target, uint32_t new_size) {
    if (!ps || !target) return -1;
    if (ensure_capacity(ps) != 0) return -1;
    FlashPatchOp *op = &ps->ops[ps->count++];
    memset(op, 0, sizeof(*op));
    op->op = PATCH_OP_RESIZE;
    strncpy(op->target, target, sizeof(op->target) - 1);
    op->new_size = new_size;
    return 0;
}

int flashpatch_add_rebase(FlashPatchSet *ps, const char *target, uint32_t new_address) {
    if (!ps || !target) return -1;
    if (ensure_capacity(ps) != 0) return -1;
    FlashPatchOp *op = &ps->ops[ps->count++];
    memset(op, 0, sizeof(*op));
    op->op = PATCH_OP_REBASE;
    strncpy(op->target, target, sizeof(op->target) - 1);
    op->new_address = new_address;
    return 0;
}

int flashpatch_add_region(FlashPatchSet *ps, const FlashRegion *region) {
    if (!ps || !region) return -1;
    if (ensure_capacity(ps) != 0) return -1;
    FlashPatchOp *op = &ps->ops[ps->count++];
    memset(op, 0, sizeof(*op));
    op->op = PATCH_OP_ADD;
    op->new_region = *region;
    return 0;
}

int flashpatch_remove_region(FlashPatchSet *ps, const char *target) {
    if (!ps || !target) return -1;
    if (ensure_capacity(ps) != 0) return -1;
    FlashPatchOp *op = &ps->ops[ps->count++];
    memset(op, 0, sizeof(*op));
    op->op = PATCH_OP_REMOVE;
    strncpy(op->target, target, sizeof(op->target) - 1);
    return 0;
}

FlashLayout *flashpatch_apply(const FlashLayout *layout, const FlashPatchSet *ps) {
    if (!layout || !ps) return NULL;
    FlashLayout *out = flashlayout_clone(layout);
    if (!out) return NULL;
    for (size_t i = 0; i < ps->count; i++) {
        const FlashPatchOp *op = &ps->ops[i];
        switch (op->op) {
            case PATCH_OP_RENAME: {
                FlashRegion *r = flashlayout_find(out, op->target);
                if (r) strncpy(r->name, op->new_name, sizeof(r->name) - 1);
                break;
            }
            case PATCH_OP_RESIZE: {
                FlashRegion *r = flashlayout_find(out, op->target);
                if (r) r->size = op->new_size;
                break;
            }
            case PATCH_OP_REBASE: {
                FlashRegion *r = flashlayout_find(out, op->target);
                if (r) r->address = op->new_address;
                break;
            }
            case PATCH_OP_ADD:
                flashlayout_add_region(out, &op->new_region);
                break;
            case PATCH_OP_REMOVE:
                flashlayout_remove_region(out, op->target);
                break;
        }
    }
    return out;
}

void flashpatch_print(const FlashPatchSet *ps) {
    if (!ps) return;
    printf("PatchSet (%zu ops):\n", ps->count);
    for (size_t i = 0; i < ps->count; i++) {
        const FlashPatchOp *op = &ps->ops[i];
        switch (op->op) {
            case PATCH_OP_RENAME:  printf("  [%zu] RENAME  '%s' -> '%s'\n",      i, op->target, op->new_name);         break;
            case PATCH_OP_RESIZE:  printf("  [%zu] RESIZE  '%s' -> 0x%08X\n",   i, op->target, op->new_size);         break;
            case PATCH_OP_REBASE:  printf("  [%zu] REBASE  '%s' -> 0x%08X\n",   i, op->target, op->new_address);      break;
            case PATCH_OP_ADD:     printf("  [%zu] ADD     '%s'\n",              i, op->new_region.name);              break;
            case PATCH_OP_REMOVE:  printf("  [%zu] REMOVE  '%s'\n",              i, op->target);                       break;
        }
    }
}
