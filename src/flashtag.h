/**
 * flashtag.h - Flash region tagging and annotation support
 *
 * Allows regions to be tagged with user-defined string labels,
 * enabling grouping, filtering, and reporting by tag.
 */

#ifndef FLASHTAG_H
#define FLASHTAG_H

#include "flashregion.h"
#include <stddef.h>

#define FLASH_TAG_MAX_LEN   32
#define FLASH_TAG_MAX_COUNT 16

/**
 * FlashTagSet - a collection of string tags associated with a region.
 */
typedef struct {
    char tags[FLASH_TAG_MAX_COUNT][FLASH_TAG_MAX_LEN];
    size_t count;
} FlashTagSet;

/**
 * Initialize an empty tag set.
 */
void flashtag_init(FlashTagSet *ts);

/**
 * Add a tag to the set. Returns 1 on success, 0 if full or duplicate.
 */
int flashtag_add(FlashTagSet *ts, const char *tag);

/**
 * Remove a tag from the set. Returns 1 if removed, 0 if not found.
 */
int flashtag_remove(FlashTagSet *ts, const char *tag);

/**
 * Check if the tag set contains the given tag.
 */
int flashtag_has(const FlashTagSet *ts, const char *tag);

/**
 * Check if two tag sets share at least one common tag.
 */
int flashtag_intersects(const FlashTagSet *a, const FlashTagSet *b);

/**
 * Print all tags in the set to stdout (comma-separated).
 */
void flashtag_print(const FlashTagSet *ts);

/**
 * Clear all tags from the set.
 */
void flashtag_clear(FlashTagSet *ts);

#endif /* FLASHTAG_H */
