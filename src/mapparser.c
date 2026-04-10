#include "mapparser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* Match lines like:
 *   .text           0x08000000     0x4a20
 * or memory region summary lines:
 *   FLASH            0x08000000 0x00100000 0x4a20 100%
 */
static int parse_section_line(const char *line, FlashSection *sec)
{
    char   name[MAX_SECTION_NAME];
    uint32_t origin, length;

    /* Try GNU ld section line: NAME ORIGIN LENGTH */
    if (sscanf(line, " %63s 0x%x 0x%x", name, &origin, &length) == 3) {
        /* Skip lines that look like symbol addresses (length == 0 common) */
        if (name[0] == '.' || (name[0] >= 'A' && name[0] <= 'Z')) {
            strncpy(sec->name, name, MAX_SECTION_NAME - 1);
            sec->name[MAX_SECTION_NAME - 1] = '\0';
            sec->origin = origin;
            sec->length = length;
            sec->used   = 0;
            return 1;
        }
    }
    return 0;
}

int flashmap_parse(const char *path, FlashMap *map)
{
    if (!path || !map) {
        return -1;
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "flashmap: cannot open '%s': %s\n", path, strerror(errno));
        return -1;
    }

    memset(map, 0, sizeof(*map));

    char line[512];
    int  in_memory_map = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* Detect the memory map section header */
        if (strstr(line, "Memory Configuration") ||
            strstr(line, "Linker script and memory map")) {
            in_memory_map = 1;
            continue;
        }

        if (!in_memory_map) {
            continue;
        }

        if (map->count >= MAX_SECTIONS) {
            fprintf(stderr, "flashmap: too many sections (max %d)\n", MAX_SECTIONS);
            break;
        }

        FlashSection sec;
        if (parse_section_line(line, &sec)) {
            map->sections[map->count++] = sec;
        }
    }

    fclose(fp);
    return 0;
}

void flashmap_free(FlashMap *map)
{
    (void)map; /* static storage — nothing to free */
}
