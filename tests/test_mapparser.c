#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/mapparser.h"

/* Write a minimal fake map file to a temp path and return the path. */
static const char *write_fake_map(void)
{
    static const char *tmp = "/tmp/flashmap_test.map";
    FILE *fp = fopen(tmp, "w");
    assert(fp && "could not create temp map file");

    fprintf(fp,
        "Linker script and memory map\n"
        "\n"
        ".text           0x08000000     0x4a20\n"
        ".data           0x20000000     0x0120\n"
        ".bss            0x20000120     0x0080\n"
        "some_symbol     0x08001234          0\n" /* should be skipped */
    );
    fclose(fp);
    return tmp;
}

static void test_parse_basic(void)
{
    const char *map_path = write_fake_map();
    FlashMap map;

    int rc = flashmap_parse(map_path, &map);
    assert(rc == 0 && "parse should succeed");
    assert(map.count == 3 && "expected 3 sections");

    assert(strcmp(map.sections[0].name, ".text") == 0);
    assert(map.sections[0].origin == 0x08000000);
    assert(map.sections[0].length == 0x4a20);

    assert(strcmp(map.sections[1].name, ".data") == 0);
    assert(map.sections[1].origin == 0x20000000);
    assert(map.sections[1].length == 0x0120);

    assert(strcmp(map.sections[2].name, ".bss") == 0);

    printf("PASS test_parse_basic\n");
}

static void test_parse_bad_path(void)
{
    FlashMap map;
    int rc = flashmap_parse("/nonexistent/file.map", &map);
    assert(rc == -1 && "expected failure for missing file");
    printf("PASS test_parse_bad_path\n");
}

static void test_parse_null_args(void)
{
    FlashMap map;
    assert(flashmap_parse(NULL, &map) == -1);
    assert(flashmap_parse("/tmp/x", NULL) == -1);
    printf("PASS test_parse_null_args\n");
}

int main(void)
{
    test_parse_basic();
    test_parse_bad_path();
    test_parse_null_args();
    printf("All tests passed.\n");
    return 0;
}
