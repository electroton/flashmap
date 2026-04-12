#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashpatch.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_test_layout(void) {
    FlashLayout *l = flashlayout_create();
    assert(l);
    FlashRegion r1 = { .name = "bootloader", .address = 0x08000000, .size = 0x8000 };
    FlashRegion r2 = { .name = "application", .address = 0x08008000, .size = 0x70000 };
    FlashRegion r3 = { .name = "config",      .address = 0x08078000, .size = 0x8000 };
    flashlayout_add_region(l, &r1);
    flashlayout_add_region(l, &r2);
    flashlayout_add_region(l, &r3);
    return l;
}

static void test_create_destroy(void) {
    FlashPatchSet *ps = flashpatch_create();
    assert(ps);
    assert(ps->count == 0);
    flashpatch_destroy(ps);
    printf("PASS test_create_destroy\n");
}

static void test_rename(void) {
    FlashPatchSet *ps = flashpatch_create();
    assert(flashpatch_add_rename(ps, "bootloader", "bl") == 0);
    assert(ps->count == 1);
    assert(ps->ops[0].op == PATCH_OP_RENAME);
    assert(strcmp(ps->ops[0].new_name, "bl") == 0);

    FlashLayout *l = make_test_layout();
    FlashLayout *out = flashpatch_apply(l, ps);
    assert(out);
    assert(flashlayout_find(out, "bl") != NULL);
    assert(flashlayout_find(out, "bootloader") == NULL);

    flashlayout_destroy(l);
    flashlayout_destroy(out);
    flashpatch_destroy(ps);
    printf("PASS test_rename\n");
}

static void test_resize(void) {
    FlashPatchSet *ps = flashpatch_create();
    assert(flashpatch_add_resize(ps, "config", 0x10000) == 0);

    FlashLayout *l = make_test_layout();
    FlashLayout *out = flashpatch_apply(l, ps);
    assert(out);
    FlashRegion *r = flashlayout_find(out, "config");
    assert(r && r->size == 0x10000);

    flashlayout_destroy(l);
    flashlayout_destroy(out);
    flashpatch_destroy(ps);
    printf("PASS test_resize\n");
}

static void test_rebase(void) {
    FlashPatchSet *ps = flashpatch_create();
    assert(flashpatch_add_rebase(ps, "application", 0x08010000) == 0);

    FlashLayout *l = make_test_layout();
    FlashLayout *out = flashpatch_apply(l, ps);
    assert(out);
    FlashRegion *r = flashlayout_find(out, "application");
    assert(r && r->address == 0x08010000);

    flashlayout_destroy(l);
    flashlayout_destroy(out);
    flashpatch_destroy(ps);
    printf("PASS test_rebase\n");
}

static void test_add_remove(void) {
    FlashPatchSet *ps = flashpatch_create();
    FlashRegion extra = { .name = "scratch", .address = 0x08080000, .size = 0x4000 };
    assert(flashpatch_add_region(ps, &extra) == 0);
    assert(flashpatch_remove_region(ps, "config") == 0);

    FlashLayout *l = make_test_layout();
    FlashLayout *out = flashpatch_apply(l, ps);
    assert(out);
    assert(flashlayout_find(out, "scratch") != NULL);
    assert(flashlayout_find(out, "config")  == NULL);

    flashlayout_destroy(l);
    flashlayout_destroy(out);
    flashpatch_destroy(ps);
    printf("PASS test_add_remove\n");
}

static void test_null_safety(void) {
    assert(flashpatch_apply(NULL, NULL) == NULL);
    assert(flashpatch_add_rename(NULL, "x", "y") == -1);
    assert(flashpatch_add_resize(NULL, "x", 0) == -1);
    flashpatch_print(NULL); /* should not crash */
    printf("PASS test_null_safety\n");
}

int main(void) {
    test_create_destroy();
    test_rename();
    test_resize();
    test_rebase();
    test_add_remove();
    test_null_safety();
    printf("All flashpatch tests passed.\n");
    return 0;
}
