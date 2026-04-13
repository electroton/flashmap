#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashsegment.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start = start;
    r.size  = size;
    return r;
}

static void test_create_free(void) {
    FlashSegmentMap *map = flash_segment_map_create();
    assert(map != NULL);
    assert(map->count == 0);
    flash_segment_map_free(map);
    printf("  [PASS] test_create_free\n");
}

static void test_add_and_find_segment(void) {
    FlashSegmentMap *map = flash_segment_map_create();
    assert(flash_segment_add(map, "code", SEGMENT_TYPE_CODE) == 0);
    assert(flash_segment_add(map, "data", SEGMENT_TYPE_DATA) == 0);
    assert(map->count == 2);

    FlashSegment *seg = flash_segment_find(map, "code");
    assert(seg != NULL);
    assert(seg->type == SEGMENT_TYPE_CODE);
    assert(strcmp(seg->name, "code") == 0);

    assert(flash_segment_find(map, "nonexistent") == NULL);
    flash_segment_map_free(map);
    printf("  [PASS] test_add_and_find_segment\n");
}

static void test_duplicate_segment_rejected(void) {
    FlashSegmentMap *map = flash_segment_map_create();
    assert(flash_segment_add(map, "bss", SEGMENT_TYPE_BSS) == 0);
    assert(flash_segment_add(map, "bss", SEGMENT_TYPE_BSS) == -1);
    assert(map->count == 1);
    flash_segment_map_free(map);
    printf("  [PASS] test_duplicate_segment_rejected\n");
}

static void test_add_region_and_total_size(void) {
    FlashSegmentMap *map = flash_segment_map_create();
    flash_segment_add(map, "code", SEGMENT_TYPE_CODE);

    FlashRegion r1 = make_region(".text",  0x08000000, 0x4000);
    FlashRegion r2 = make_region(".rodata",0x08004000, 0x1000);

    assert(flash_segment_add_region(map, "code", &r1) == 0);
    assert(flash_segment_add_region(map, "code", &r2) == 0);

    FlashSegment *seg = flash_segment_find(map, "code");
    assert(seg->region_count == 2);
    assert(flash_segment_total_size(seg) == 0x5000);

    flash_segment_map_free(map);
    printf("  [PASS] test_add_region_and_total_size\n");
}

static void test_start_end_address(void) {
    FlashSegmentMap *map = flash_segment_map_create();
    flash_segment_add(map, "data", SEGMENT_TYPE_DATA);

    FlashRegion r1 = make_region(".data", 0x20000000, 0x800);
    FlashRegion r2 = make_region(".bss",  0x20000800, 0x400);

    flash_segment_add_region(map, "data", &r1);
    flash_segment_add_region(map, "data", &r2);

    FlashSegment *seg = flash_segment_find(map, "data");
    assert(flash_segment_start_address(seg) == 0x20000000);
    assert(flash_segment_end_address(seg)   == 0x20000C00);

    flash_segment_map_free(map);
    printf("  [PASS] test_start_end_address\n");
}

static void test_type_string_roundtrip(void) {
    const char *types[] = {"code", "data", "bss", "rodata", "stack", "heap", "custom"};
    for (size_t i = 0; i < sizeof(types)/sizeof(types[0]); i++) {
        FlashSegmentType t = flash_segment_type_from_string(types[i]);
        assert(strcmp(flash_segment_type_to_string(t), types[i]) == 0);
    }
    assert(flash_segment_type_from_string("garbage") == SEGMENT_TYPE_UNKNOWN);
    assert(strcmp(flash_segment_type_to_string(SEGMENT_TYPE_UNKNOWN), "unknown") == 0);
    printf("  [PASS] test_type_string_roundtrip\n");
}

static void test_add_region_to_unknown_segment(void) {
    FlashSegmentMap *map = flash_segment_map_create();
    FlashRegion r = make_region(".text", 0x08000000, 0x1000);
    assert(flash_segment_add_region(map, "missing", &r) == -1);
    flash_segment_map_free(map);
    printf("  [PASS] test_add_region_to_unknown_segment\n");
}

int main(void) {
    printf("Running flashsegment tests...\n");
    test_create_free();
    test_add_and_find_segment();
    test_duplicate_segment_rejected();
    test_add_region_and_total_size();
    test_start_end_address();
    test_type_string_roundtrip();
    test_add_region_to_unknown_segment();
    printf("All flashsegment tests passed.\n");
    return 0;
}
