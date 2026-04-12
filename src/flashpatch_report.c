#include <stdio.h>
#include <string.h>
#include "flashpatch_report.h"

static const char *op_name(FlashPatchOpType op) {
    switch (op) {
        case PATCH_OP_RENAME:  return "RENAME";
        case PATCH_OP_RESIZE:  return "RESIZE";
        case PATCH_OP_REBASE:  return "REBASE";
        case PATCH_OP_ADD:     return "ADD";
        case PATCH_OP_REMOVE:  return "REMOVE";
        default:               return "UNKNOWN";
    }
}

void flashpatch_report(const FlashLayout *original,
                       const FlashLayout *patched,
                       const FlashPatchSet *ps) {
    if (!original || !patched || !ps) return;

    printf("=== Flash Patch Report ===\n");
    printf("Operations applied: %zu\n\n", ps->count);

    for (size_t i = 0; i < ps->count; i++) {
        const FlashPatchOp *op = &ps->ops[i];
        printf("  %zu. %-8s", i + 1, op_name(op->op));
        switch (op->op) {
            case PATCH_OP_RENAME:
                printf(" '%s' -> '%s'", op->target, op->new_name);
                break;
            case PATCH_OP_RESIZE: {
                const FlashRegion *orig = flashlayout_find_const(original, op->target);
                if (orig)
                    printf(" '%s': 0x%08X -> 0x%08X (%+d bytes)",
                           op->target, orig->size, op->new_size,
                           (int)op->new_size - (int)orig->size);
                else
                    printf(" '%s': -> 0x%08X", op->target, op->new_size);
                break;
            }
            case PATCH_OP_REBASE: {
                const FlashRegion *orig = flashlayout_find_const(original, op->target);
                if (orig)
                    printf(" '%s': 0x%08X -> 0x%08X",
                           op->target, orig->address, op->new_address);
                else
                    printf(" '%s': -> 0x%08X", op->target, op->new_address);
                break;
            }
            case PATCH_OP_ADD:
                printf(" '%s' @ 0x%08X size 0x%08X",
                       op->new_region.name, op->new_region.address, op->new_region.size);
                break;
            case PATCH_OP_REMOVE:
                printf(" '%s'", op->target);
                break;
        }
        printf("\n");
    }

    printf("\nOriginal regions: %zu\n", (size_t)original->count);
    printf("Patched  regions: %zu\n", (size_t)patched->count);
    printf("=========================\n");
}
