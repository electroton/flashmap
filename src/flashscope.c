#include "flashscope.h"
#include <string.h>
#include <stdio.h>

void flashscope_init(FlashScopeSet *set) {
    if (!set) return;
    memset(set, 0, sizeof(*set));
}

int flashscope_add(FlashScopeSet *set, const char *name,
                   uint32_t base_addr, uint32_t size) {
    if (!set || !name) return -1;
    if (set->count >= FLASHSCOPE_MAX_SCOPES) return -1;

    FlashScope *s = &set->scopes[set->count];
    strncpy(s->name, name, FLASHSCOPE_MAX_NAME - 1);
    s->name[FLASHSCOPE_MAX_NAME - 1] = '\0';
    s->base_addr = base_addr;
    s->size      = size;
    s->active    = true;
    set->count++;
    return 0;
}

FlashScope *flashscope_find(FlashScopeSet *set, const char *name) {
    if (!set || !name) return NULL;
    for (int i = 0; i < set->count; i++) {
        if (strncmp(set->scopes[i].name, name, FLASHSCOPE_MAX_NAME) == 0) {
            return &set->scopes[i];
        }
    }
    return NULL;
}

bool flashscope_contains(const FlashScopeSet *set, uint32_t addr) {
    if (!set) return false;
    for (int i = 0; i < set->count; i++) {
        const FlashScope *s = &set->scopes[i];
        if (!s->active) continue;
        if (addr >= s->base_addr && addr < s->base_addr + s->size) {
            return true;
        }
    }
    return false;
}

int flashscope_filter_layout(const FlashScopeSet *set,
                             const FlashLayout *layout,
                             FlashLayout *out_layout) {
    if (!set || !layout || !out_layout) return 0;

    int written = 0;
    flashlayout_init(out_layout);

    for (int i = 0; i < layout->count; i++) {
        const FlashRegion *r = &layout->regions[i];
        /* Include region if its start address falls within any active scope */
        if (flashscope_contains(set, r->start_addr)) {
            if (flashlayout_add_region(out_layout, r) == 0) {
                written++;
            }
        }
    }
    return written;
}

int flashscope_set_active(FlashScopeSet *set, const char *name, bool active) {
    FlashScope *s = flashscope_find(set, name);
    if (!s) return -1;
    s->active = active;
    return 0;
}
