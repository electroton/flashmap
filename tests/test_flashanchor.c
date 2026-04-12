#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashanchor.h"
#include "../src/flashregion.h"

/* Minimal FlashRegion helpers for tests */
static FlashRegion make_region(const char *name, uint32_t addr, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.address = addr;
    r.size    = size;
    return r;
}

static void test_init_and_add(void) {
    FlashAnchorSet set;
    flash_anchor_set_init(&set);
    assert(set.count == 0);

    int rc = flash_anchor_add(&set, "reset_vector", 0x08000000, 0,
                              ANCHOR_TYPE_ENTRY, 1);
    assert(rc == 0);
    assert(set.count == 1);
    assert(set.entries[0].address == 0x08000000);
    assert(set.entries[0].required == 1);
    printf("PASS: test_init_and_add\n");
}

static void test_find(void) {
    FlashAnchorSet set;
    flash_anchor_set_init(&set);
    flash_anchor_add(&set, "ivt", 0x08000000, 0x200, ANCHOR_TYPE_VECTOR, 1);
    flash_anchor_add(&set, "cfg", 0x0803F000, 0x1000, ANCHOR_TYPE_CONFIG, 0);

    const FlashAnchor *a = flash_anchor_find(&set, "ivt");
    assert(a != NULL);
    assert(a->address == 0x08000000);

    const FlashAnchor *b = flash_anchor_find(&set, "nonexistent");
    assert(b == NULL);
    printf("PASS: test_find\n");
}

static void test_validate_all_satisfied(void) {
    FlashAnchorSet set;
    flash_anchor_set_init(&set);
    flash_anchor_add(&set, "reset", 0x08000004, 0, ANCHOR_TYPE_ENTRY, 1);
    flash_anchor_add(&set, "ivt",   0x08000000, 0x200, ANCHOR_TYPE_VECTOR, 1);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 0x08000000, 0x40000);

    char buf[512];
    int v = flash_anchor_validate(&set, regions, 1, buf, sizeof(buf));
    assert(v == 0);
    printf("PASS: test_validate_all_satisfied\n");
}

static void test_validate_violation(void) {
    FlashAnchorSet set;
    flash_anchor_set_init(&set);
    /* Anchor outside the defined region */
    flash_anchor_add(&set, "bootrom", 0x1FFF0000, 0, ANCHOR_TYPE_ENTRY, 1);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 0x08000000, 0x40000);

    char buf[512];
    int v = flash_anchor_validate(&set, regions, 1, buf, sizeof(buf));
    assert(v == 1);
    assert(strstr(buf, "bootrom") != NULL);
    assert(strstr(buf, "ERROR") != NULL);
    printf("PASS: test_validate_violation\n");
}

static void test_validate_optional_violation(void) {
    FlashAnchorSet set;
    flash_anchor_set_init(&set);
    flash_anchor_add(&set, "opt_cfg", 0x0807F000, 0, ANCHOR_TYPE_CONFIG, 0);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 0x08000000, 0x40000);

    char buf[512];
    int v = flash_anchor_validate(&set, regions, 1, buf, sizeof(buf));
    assert(v == 1);
    assert(strstr(buf, "WARN") != NULL);
    printf("PASS: test_validate_optional_violation\n");
}

static void test_anchor_type_str(void) {
    assert(strcmp(flash_anchor_type_str(ANCHOR_TYPE_ENTRY),    "ENTRY")    == 0);
    assert(strcmp(flash_anchor_type_str(ANCHOR_TYPE_VECTOR),   "VECTOR")   == 0);
    assert(strcmp(flash_anchor_type_str(ANCHOR_TYPE_CONFIG),   "CONFIG")   == 0);
    assert(strcmp(flash_anchor_type_str(ANCHOR_TYPE_BOUNDARY), "BOUNDARY") == 0);
    assert(strcmp(flash_anchor_type_str(ANCHOR_TYPE_CUSTOM),   "CUSTOM")   == 0);
    printf("PASS: test_anchor_type_str\n");
}

int main(void) {
    test_init_and_add();
    test_find();
    test_validate_all_satisfied();
    test_validate_violation();
    test_validate_optional_violation();
    test_anchor_type_str();
    printf("All flashanchor tests passed.\n");
    return 0;
}
