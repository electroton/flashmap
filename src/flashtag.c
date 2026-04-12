#include "flashtag.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void flashtag_init(FlashTagRegistry *registry) {
    if (!registry) return;
    registry->count = 0;
    memset(registry->tags, 0, sizeof(registry->tags));
}

int flashtag_add(FlashTagRegistry *registry, const char *region_name, const char *tag) {
    if (!registry || !region_name || !tag) return FLASHTAG_ERR_NULL;
    if (registry->count >= FLASHTAG_MAX_TAGS) return FLASHTAG_ERR_FULL;
    if (strlen(region_name) >= FLASHTAG_NAME_LEN) return FLASHTAG_ERR_NAME_TOO_LONG;
    if (strlen(tag) >= FLASHTAG_TAG_LEN) return FLASHTAG_ERR_TAG_TOO_LONG;

    /* Check for duplicate */
    for (int i = 0; i < registry->count; i++) {
        if (strcmp(registry->tags[i].region_name, region_name) == 0 &&
            strcmp(registry->tags[i].tag, tag) == 0) {
            return FLASHTAG_ERR_DUPLICATE;
        }
    }

    strncpy(registry->tags[registry->count].region_name, region_name, FLASHTAG_NAME_LEN - 1);
    strncpy(registry->tags[registry->count].tag, tag, FLASHTAG_TAG_LEN - 1);
    registry->tags[registry->count].region_name[FLASHTAG_NAME_LEN - 1] = '\0';
    registry->tags[registry->count].tag[FLASHTAG_TAG_LEN - 1] = '\0';
    registry->count++;
    return FLASHTAG_OK;
}

int flashtag_remove(FlashTagRegistry *registry, const char *region_name, const char *tag) {
    if (!registry || !region_name || !tag) return FLASHTAG_ERR_NULL;
    for (int i = 0; i < registry->count; i++) {
        if (strcmp(registry->tags[i].region_name, region_name) == 0 &&
            strcmp(registry->tags[i].tag, tag) == 0) {
            registry->tags[i] = registry->tags[registry->count - 1];
            registry->count--;
            return FLASHTAG_OK;
        }
    }
    return FLASHTAG_ERR_NOT_FOUND;
}

int flashtag_get_tags_for_region(const FlashTagRegistry *registry, const char *region_name,
                                  char out_tags[][FLASHTAG_TAG_LEN], int max_out) {
    if (!registry || !region_name || !out_tags) return 0;
    int found = 0;
    for (int i = 0; i < registry->count && found < max_out; i++) {
        if (strcmp(registry->tags[i].region_name, region_name) == 0) {
            strncpy(out_tags[found], registry->tags[i].tag, FLASHTAG_TAG_LEN - 1);
            out_tags[found][FLASHTAG_TAG_LEN - 1] = '\0';
            found++;
        }
    }
    return found;
}

int flashtag_has_tag(const FlashTagRegistry *registry, const char *region_name, const char *tag) {
    if (!registry || !region_name || !tag) return 0;
    for (int i = 0; i < registry->count; i++) {
        if (strcmp(registry->tags[i].region_name, region_name) == 0 &&
            strcmp(registry->tags[i].tag, tag) == 0) {
            return 1;
        }
    }
    return 0;
}

void flashtag_print(const FlashTagRegistry *registry) {
    if (!registry) return;
    printf("FlashTagRegistry (%d entries):\n", registry->count);
    for (int i = 0; i < registry->count; i++) {
        printf("  [%s] -> \"%s\"\n", registry->tags[i].region_name, registry->tags[i].tag);
    }
}
