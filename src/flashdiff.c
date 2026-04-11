#include "flashdiff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8

static int diff_append(FlashDiff *diff, const FlashDiffEntry *entry) {
    if (diff->count >= diff->capacity) {
        size_t new_cap = diff->capacity * 2;
        FlashDiffEntry *tmp = realloc(diff->entries,
                                      new_cap * sizeof(FlashDiffEntry));
        if (!tmp) return -1;
        diff->entries  = tmp;
        diff->capacity = new_cap;
    }
    diff->entries[diff->count++] = *entry;
    return 0;
}

int flashdiff_compare(const FlashLayout *old_layout,
                      const FlashLayout *new_layout,
                      FlashDiff *out_diff) {
    if (!old_layout || !new_layout || !out_diff) return -1;

    out_diff->entries = malloc(INITIAL_CAPACITY * sizeof(FlashDiffEntry));
    if (!out_diff->entries) return -1;
    out_diff->count            = 0;
    out_diff->capacity         = INITIAL_CAPACITY;
    out_diff->total_size_delta = 0;

    /* Check for removed or modified regions */
    for (size_t i = 0; i < old_layout->count; i++) {
        const FlashRegion *old_r = &old_layout->regions[i];
        FlashDiffEntry entry;
        memset(&entry, 0, sizeof(entry));
        strncpy(entry.name, old_r->name, sizeof(entry.name) - 1);
        entry.old_address = old_r->address;
        entry.old_size    = old_r->size;

        const FlashRegion *new_r = flashlayout_find(new_layout, old_r->name);
        if (!new_r) {
            entry.type        = DIFF_REMOVED;
            entry.size_delta  = -(int32_t)old_r->size;
        } else if (new_r->address != old_r->address ||
                   new_r->size    != old_r->size) {
            entry.type        = DIFF_MODIFIED;
            entry.new_address = new_r->address;
            entry.new_size    = new_r->size;
            entry.size_delta  = (int32_t)new_r->size - (int32_t)old_r->size;
        } else {
            entry.type        = DIFF_UNCHANGED;
            entry.new_address = new_r->address;
            entry.new_size    = new_r->size;
            entry.size_delta  = 0;
        }
        out_diff->total_size_delta += entry.size_delta;
        if (diff_append(out_diff, &entry) != 0) return -1;
    }

    /* Check for added regions */
    for (size_t j = 0; j < new_layout->count; j++) {
        const FlashRegion *new_r = &new_layout->regions[j];
        if (!flashlayout_find(old_layout, new_r->name)) {
            FlashDiffEntry entry;
            memset(&entry, 0, sizeof(entry));
            strncpy(entry.name, new_r->name, sizeof(entry.name) - 1);
            entry.type            = DIFF_ADDED;
            entry.new_address     = new_r->address;
            entry.new_size        = new_r->size;
            entry.size_delta      = (int32_t)new_r->size;
            out_diff->total_size_delta += entry.size_delta;
            if (diff_append(out_diff, &entry) != 0) return -1;
        }
    }
    return 0;
}

void flashdiff_print(const FlashDiff *diff) {
    if (!diff) return;
    static const char *labels[] = {"ADDED", "REMOVED", "MODIFIED", "UNCHANGED"};
    printf("%-20s %-10s %10s %10s %10s\n",
           "Region", "Status", "OldSize", "NewSize", "Delta");
    printf("%-20s %-10s %10s %10s %10s\n",
           "------", "------", "-------", "-------", "-----");
    for (size_t i = 0; i < diff->count; i++) {
        const FlashDiffEntry *e = &diff->entries[i];
        printf("%-20s %-10s %10u %10u %+10d\n",
               e->name, labels[e->type],
               e->old_size, e->new_size, e->size_delta);
    }
    printf("\nTotal size delta: %+d bytes\n", diff->total_size_delta);
}

void flashdiff_free(FlashDiff *diff) {
    if (!diff) return;
    free(diff->entries);
    diff->entries  = NULL;
    diff->count    = 0;
    diff->capacity = 0;
}
