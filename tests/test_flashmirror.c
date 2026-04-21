#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashmirror.h"
#include "../src/flashmirror_report.h"

static FlashRegion make_region(const char *name, uint32_t base, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.base_address = base;
    r.size = size;
    return r;
}

static void test_create_free(void) {
    FlashMirrorMap *map = flash_mirror_map_create();
    assert(map != NULL);
    assert(map->count == 0);
    flash_mirror_map_free(map);
    printf("  PASS: test_create_free\n");
}

static void test_add_pair(void) {
    FlashMirrorMap *map = flash_mirror_map_create();
    FlashRegion primary = make_region("FLASH_BANK0", 0x08000000, 0x20000);
    FlashRegion mirror  = make_region("FLASH_BANK1", 0x08020000, 0x20000);

    int rc = flash_mirror_add(map, &primary, &mirror);
    assert(rc == 0);
    assert(map->count == 1);
    assert(strcmp(map->pairs[0].primary_name, "FLASH_BANK0") == 0);
    assert(strcmp(map->pairs[0].mirror_name,  "FLASH_BANK1") == 0);
    assert(map->pairs[0].size == 0x20000);
    assert(map->pairs[0].active == true);

    flash_mirror_map_free(map);
    printf("  PASS: test_add_pair\n");
}

static void test_add_pair_size_mismatch(void) {
    FlashMirrorMap *map = flash_mirror_map_create();
    FlashRegion a = make_region("A", 0x08000000, 0x10000);
    FlashRegion b = make_region("B", 0x08020000, 0x20000);
    int rc = flash_mirror_add(map, &a, &b);
    assert(rc == -1);
    assert(map->count == 0);
    flash_mirror_map_free(map);
    printf("  PASS: test_add_pair_size_mismatch\n");
}

static void test_is_pair(void) {
    FlashRegion a = make_region("A", 0x08000000, 0x10000);
    FlashRegion b = make_region("B", 0x08010000, 0x10000);
    FlashRegion c = make_region("C", 0x08020000, 0x20000);
    assert(flash_mirror_is_pair(&a, &b) == true);
    assert(flash_mirror_is_pair(&a, &c) == false);
    assert(flash_mirror_is_pair(&a, &a) == false);
    printf("  PASS: test_is_pair\n");
}

static void test_find(void) {
    FlashMirrorMap *map = flash_mirror_map_create();
    FlashRegion p = make_region("PRIMARY", 0x08000000, 0x8000);
    FlashRegion m = make_region("MIRROR",  0x08008000, 0x8000);
    flash_mirror_add(map, &p, &m);

    const FlashMirrorPair *found = flash_mirror_find(map, "PRIMARY");
    assert(found != NULL);
    assert(strcmp(found->mirror_name, "MIRROR") == 0);

    found = flash_mirror_find(map, "MIRROR");
    assert(found != NULL);
    assert(strcmp(found->primary_name, "PRIMARY") == 0);

    found = flash_mirror_find(map, "NONEXISTENT");
    assert(found == NULL);

    flash_mirror_map_free(map);
    printf("  PASS: test_find\n");
}

static void test_detect(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.regions = malloc(sizeof(FlashRegion) * 4);
    layout.region_count = 4;
    layout.regions[0] = make_region("BOOT",   0x08000000, 0x4000);
    layout.regions[1] = make_region("BOOT_M", 0x08010000, 0x4000);
    layout.regions[2] = make_region("APP",    0x08020000, 0x10000);
    layout.regions[3] = make_region("DATA",   0x08030000, 0x2000);

    FlashMirrorMap *map = flash_mirror_map_create();
    int found = flash_mirror_detect(map, &layout);
    assert(found == 1);
    assert(map->count == 1);

    free(layout.regions);
    flash_mirror_map_free(map);
    printf("  PASS: test_detect\n");
}

static void test_report(void) {
    FlashMirrorMap *map = flash_mirror_map_create();
    FlashRegion p = make_region("FLASH_PRIMARY", 0x08000000, 0x40000);
    FlashRegion m = make_region("FLASH_MIRROR",  0x08040000, 0x40000);
    flash_mirror_add(map, &p, &m);
    flash_mirror_report(map, stdout);
    flash_mirror_map_free(map);
    printf("  PASS: test_report\n");
}

int main(void) {
    printf("Running flashmirror tests...\n");
    test_create_free();
    test_add_pair();
    test_add_pair_size_mismatch();
    test_is_pair();
    test_find();
    test_detect();
    test_report();
    printf("All flashmirror tests passed.\n");
    return 0;
}
