#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "flashanchor.h"

void flash_anchor_set_init(FlashAnchorSet *set) {
    if (!set) return;
    memset(set, 0, sizeof(*set));
}

int flash_anchor_add(FlashAnchorSet *set, const char *name,
                     uint32_t address, uint32_t size,
                     FlashAnchorType type, int required) {
    if (!set || !name || set->count >= FLASH_ANCHOR_MAX)
        return -1;

    FlashAnchor *a = &set->entries[set->count];
    strncpy(a->name, name, FLASH_ANCHOR_NAME_MAX - 1);
    a->name[FLASH_ANCHOR_NAME_MAX - 1] = '\0';
    a->address  = address;
    a->size     = size;
    a->type     = type;
    a->required = required;
    set->count++;
    return 0;
}

const FlashAnchor *flash_anchor_find(const FlashAnchorSet *set,
                                     const char *name) {
    if (!set || !name) return NULL;
    for (size_t i = 0; i < set->count; i++) {
        if (strcmp(set->entries[i].name, name) == 0)
            return &set->entries[i];
    }
    return NULL;
}

int flash_anchor_validate(const FlashAnchorSet *set,
                          const FlashRegion *regions, size_t count,
                          char *report_buf, size_t report_buf_len) {
    if (!set || !regions) return -1;

    int violations = 0;
    size_t written = 0;
    if (report_buf && report_buf_len > 0)
        report_buf[0] = '\0';

    for (size_t i = 0; i < set->count; i++) {
        const FlashAnchor *a = &set->entries[i];
        int matched = 0;

        for (size_t j = 0; j < count; j++) {
            uint32_t r_start = regions[j].address;
            uint32_t r_end   = r_start + regions[j].size;

            if (a->size == 0) {
                /* Point anchor: address must fall within some region */
                if (a->address >= r_start && a->address < r_end) {
                    matched = 1;
                    break;
                }
            } else {
                /* Range anchor: anchor range must be covered by a region */
                uint32_t a_end = a->address + a->size;
                if (a->address >= r_start && a_end <= r_end) {
                    matched = 1;
                    break;
                }
            }
        }

        if (!matched) {
            violations++;
            if (report_buf && written < report_buf_len - 1) {
                int n = snprintf(report_buf + written, report_buf_len - written,
                                 "[%s] anchor '%s' @ 0x%08X not satisfied\n",
                                 a->required ? "ERROR" : "WARN",
                                 a->name, a->address);
                if (n > 0) written += (size_t)n;
            }
        }
    }
    return violations;
}

void flash_anchor_print(const FlashAnchorSet *set) {
    if (!set) return;
    printf("Flash Anchors (%zu):\n", set->count);
    for (size_t i = 0; i < set->count; i++) {
        const FlashAnchor *a = &set->entries[i];
        printf("  %-32s  0x%08X  size=%-6u  type=%-8s  %s\n",
               a->name, a->address, a->size,
               flash_anchor_type_str(a->type),
               a->required ? "required" : "optional");
    }
}

const char *flash_anchor_type_str(FlashAnchorType type) {
    switch (type) {
        case ANCHOR_TYPE_ENTRY:    return "ENTRY";
        case ANCHOR_TYPE_VECTOR:   return "VECTOR";
        case ANCHOR_TYPE_CONFIG:   return "CONFIG";
        case ANCHOR_TYPE_BOUNDARY: return "BOUNDARY";
        case ANCHOR_TYPE_CUSTOM:   return "CUSTOM";
        default:                   return "UNKNOWN";
    }
}
