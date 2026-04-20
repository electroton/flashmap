#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashpin.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t start, uint32_t size) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.start  = start;
    r.size   = size;
    r.end    = start + size - 1;
    return r;
}

static void test_init(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    assert(ps.count == 0);
    printf("[PASS] test_init\n");
}

static void test_add_address(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    assert(flash_pin_add_address(&ps, "reset_vector", 0x08000000));
    assert(ps.count == 1);
    assert(ps.pins[0].type == FLASH_PIN_TYPE_ADDRESS);
    assert(ps.pins[0].address == 0x08000000);
    assert(strcmp(ps.pins[0].label, "reset_vector") == 0);
    printf("[PASS] test_add_address\n");
}

static void test_add_region(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    FlashRegion r = make_region("bootloader", 0x08000000, 0x4000);
    assert(flash_pin_add_region(&ps, "boot_pin", &r));
    assert(ps.count == 1);
    assert(ps.pins[0].type == FLASH_PIN_TYPE_REGION);
    assert(ps.pins[0].address == 0x08000000);
    assert(strcmp(ps.pins[0].region_name, "bootloader") == 0);
    printf("[PASS] test_add_region\n");
}

static void test_add_offset(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    FlashRegion r = make_region("app", 0x08004000, 0x10000);
    assert(flash_pin_add_offset(&ps, "isr_table", &r, 0x100));
    assert(ps.pins[0].type == FLASH_PIN_TYPE_OFFSET);
    assert(ps.pins[0].address == 0x08004100);
    assert(ps.pins[0].offset  == 0x100);
    printf("[PASS] test_add_offset\n");
}

static void test_find_and_remove(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    flash_pin_add_address(&ps, "marker_a", 0x1000);
    flash_pin_add_address(&ps, "marker_b", 0x2000);

    const FlashPin *p = flash_pin_find(&ps, "marker_a");
    assert(p != NULL);
    assert(p->address == 0x1000);

    assert(flash_pin_remove(&ps, "marker_a"));
    assert(ps.count == 1);
    assert(flash_pin_find(&ps, "marker_a") == NULL);
    assert(!flash_pin_remove(&ps, "nonexistent"));
    printf("[PASS] test_find_and_remove\n");
}

static void test_hits_address(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    flash_pin_add_address(&ps, "cksum", 0x0800FFFC);

    const FlashPin *hit = NULL;
    assert(flash_pin_hits_address(&ps, 0x0800FFFC, &hit));
    assert(hit != NULL && strcmp(hit->label, "cksum") == 0);
    assert(!flash_pin_hits_address(&ps, 0x0800FFFD, &hit));
    printf("[PASS] test_hits_address\n");
}

static void test_capacity_limit(void) {
    FlashPinSet ps;
    flash_pin_set_init(&ps);
    for (int i = 0; i < FLASH_PIN_MAX_PINS; i++) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "pin_%d", i);
        assert(flash_pin_add_address(&ps, lbl, (uint32_t)(i * 4)));
    }
    assert(ps.count == FLASH_PIN_MAX_PINS);
    assert(!flash_pin_add_address(&ps, "overflow", 0xDEAD));
    printf("[PASS] test_capacity_limit\n");
}

int main(void) {
    test_init();
    test_add_address();
    test_add_region();
    test_add_offset();
    test_find_and_remove();
    test_hits_address();
    test_capacity_limit();
    printf("All flashpin tests passed.\n");
    return 0;
}
