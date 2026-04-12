ifndef MAPPARSER_H
#define MAPPARSER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_SECTION_NAME 64
#define MAX_SECTIONS     256

typedef struct {
    char     name[MAX_SECTION_NAME];
    uint32_t origin;
    uint32_t length;
    uint32_t used;
} FlashSection;

typedef struct {
    FlashSection sections[MAX_SECTIONS];
    size_t       count;
} FlashMap;

/**
 * Parse a GNU ld linker map file and populate a FlashMap.
 *
 * @param path  Path to the .map file.
 * @param map   Output structure to fill.
 * @return      0 on success, -1 on error.
 */
int flashmap_parse(const char *path, FlashMap *map);

/**
 * Look up a section by name in a FlashMap.
 *
 * @param map   Pointer to a populated FlashMap.
 * @param name  Name of the section to find.
 * @return      Pointer to the matching FlashSection, or NULL if not found.
 */
const FlashSection *flashmap_find_section(const FlashMap *map, const char *name);

/**
 * Free any resources associated with a FlashMap.
 * (Currently a no-op for static storage, kept for API stability.)
 */
void flashmap_free(FlashMap *map);

#endif /* MAPPARSER_H */
