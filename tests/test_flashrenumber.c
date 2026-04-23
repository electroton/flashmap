#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashrenumber.h"
#include "../src/flashregion.h"

static void make_region(FlashRegion *r, const char *name,
                        uint32_t id, uint32_t base, uint32_t size) {
    memset(r, 0, sizeof(*r));
    strncpy(r->name, name, sizeof(r->name) - 1);
    r->id           = id;
    r->base_address = base;
    r->size         = size;
}

static void test_basic_renumber(void) {
    FlashRegion regions[3];
    make_region(&regions[0], "boot",   10, 0x0000, 0x1000);
    make_region(&regions[1], "app",    20, 0x1000, 0x8000);
    make_region(&regions[2], "config", 30, 0x9000, 0x0400);

    FlashRenumberOptions opts = { .start_id = 1, .step = 1, .apply = 0 };
    FlashRenumberResult  res  = flash_renumber(regions, 3, &opts);

    assert(res.status == FLASH_RENUMBER_OK);
    assert(res.count  == 3);
    assert(res.entries[0].new_id == 1);
    assert(res.entries[1].new_id == 2);
    assert(res.entries[2].new_id == 3);

    /* apply=0: original IDs unchanged */
    assert(regions[0].id == 10);
    assert(regions[1].id == 20);
    assert(regions[2].id == 30);

    flash_renumber_result_free(&res);
    printf("PASS: test_basic_renumber\n");
}

static void test_renumber_with_step(void) {
    FlashRegion regions[4];
    for (int i = 0; i < 4; i++) {
        char name[16];
        snprintf(name, sizeof(name), "r%d", i);
        make_region(&regions[i], name, (uint32_t)i, (uint32_t)(i * 0x1000), 0x1000);
    }

    FlashRenumberOptions opts = { .start_id = 100, .step = 10, .apply = 1 };
    FlashRenumberResult  res  = flash_renumber(regions, 4, &opts);

    assert(res.status == FLASH_RENUMBER_OK);
    assert(res.entries[0].new_id == 100);
    assert(res.entries[1].new_id == 110);
    assert(res.entries[2].new_id == 120);
    assert(res.entries[3].new_id == 130);

    /* apply=1: IDs written back */
    assert(regions[0].id == 100);
    assert(regions[3].id == 130);

    flash_renumber_result_free(&res);
    printf("PASS: test_renumber_with_step\n");
}

static void test_renumber_by_address(void) {
    FlashRegion regions[3];
    /* Intentionally out of address order */
    make_region(&regions[0], "config", 1, 0x9000, 0x0400);
    make_region(&regions[1], "boot",   2, 0x0000, 0x1000);
    make_region(&regions[2], "app",    3, 0x1000, 0x8000);

    FlashRenumberOptions opts = { .start_id = 0, .step = 1, .apply = 0 };
    FlashRenumberResult  res  = flash_renumber_by_address(regions, 3, &opts);

    assert(res.status == FLASH_RENUMBER_OK);
    assert(res.count  == 3);

    /* Entries should be in ascending address order */
    assert(res.entries[0].region->base_address == 0x0000);
    assert(res.entries[1].region->base_address == 0x1000);
    assert(res.entries[2].region->base_address == 0x9000);

    assert(res.entries[0].new_id == 0);
    assert(res.entries[1].new_id == 1);
    assert(res.entries[2].new_id == 2);

    flash_renumber_result_free(&res);
    printf("PASS: test_renumber_by_address\n");
}

static void test_invalid_args(void) {
    FlashRenumberOptions opts = { .start_id = 1, .step = 1, .apply = 0 };

    FlashRenumberResult r1 = flash_renumber(NULL, 3, &opts);
    assert(r1.status == FLASH_RENUMBER_ERR_INVALID);

    FlashRegion regions[2];
    make_region(&regions[0], "a", 0, 0x0000, 0x1000);
    make_region(&regions[1], "b", 1, 0x1000, 0x1000);

    FlashRenumberResult r2 = flash_renumber(regions, 0, &opts);
    assert(r2.status == FLASH_RENUMBER_ERR_INVALID);

    FlashRenumberResult r3 = flash_renumber(regions, 2, NULL);
    assert(r3.status == FLASH_RENUMBER_ERR_INVALID);

    printf("PASS: test_invalid_args\n");
}

int main(void) {
    test_basic_renumber();
    test_renumber_with_step();
    test_renumber_by_address();
    test_invalid_args();
    printf("All flashrenumber tests passed.\n");
    return 0;
}
