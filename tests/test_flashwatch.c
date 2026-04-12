/*
 * test_flashwatch.c - Unit tests for flashwatch module
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashwatch.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout *make_layout(void) {
    FlashLayout *l = calloc(1, sizeof(FlashLayout));
    l->capacity = 4;
    l->regions = calloc(l->capacity, sizeof(FlashRegion));
    return l;
}

static void add_region(FlashLayout *l, const char *name, uint32_t base, uint32_t size) {
    FlashRegion *r = &l->regions[l->count++];
    strncpy(r->name, name, sizeof(r->name) - 1);
    r->base = base;
    r->size = size;
}

static void free_layout(FlashLayout *l) {
    free(l->regions);
    free(l);
}

static void test_create_destroy(void) {
    FlashWatch *w = flashwatch_create();
    assert(w != NULL);
    assert(w->count == 0);
    flashwatch_destroy(w);
    printf("  [PASS] test_create_destroy\n");
}

static void test_add_remove(void) {
    FlashWatch *w = flashwatch_create();
    assert(flashwatch_add(w, "FLASH", 0x08000000, 0x80000) == 0);
    assert(flashwatch_add(w, "RAM",   0x20000000, 0x20000) == 0);
    assert(w->count == 2);
    assert(flashwatch_remove(w, "FLASH") == 0);
    assert(w->count == 1);
    assert(strcmp(w->entries[0].region_name, "RAM") == 0);
    assert(flashwatch_remove(w, "NONEXISTENT") == -1);
    flashwatch_destroy(w);
    printf("  [PASS] test_add_remove\n");
}

static void test_check_no_change(void) {
    FlashWatch *w = flashwatch_create();
    flashwatch_add(w, "FLASH", 0x08000000, 0x80000);
    FlashLayout *l = make_layout();
    add_region(l, "FLASH", 0x08000000, 0x80000);
    int triggered = flashwatch_check(w, l);
    assert(triggered == 0);
    assert(w->entries[0].triggered == false);
    free_layout(l);
    flashwatch_destroy(w);
    printf("  [PASS] test_check_no_change\n");
}

static void test_check_size_change(void) {
    FlashWatch *w = flashwatch_create();
    flashwatch_add(w, "FLASH", 0x08000000, 0x80000);
    FlashLayout *l = make_layout();
    add_region(l, "FLASH", 0x08000000, 0x90000); /* size changed */
    int triggered = flashwatch_check(w, l);
    assert(triggered == 1);
    assert(w->entries[0].triggered == true);
    assert(w->entries[0].prev_size == 0x90000);
    free_layout(l);
    flashwatch_destroy(w);
    printf("  [PASS] test_check_size_change\n");
}

static void test_reset(void) {
    FlashWatch *w = flashwatch_create();
    flashwatch_add(w, "FLASH", 0x08000000, 0x80000);
    w->entries[0].triggered = true;
    flashwatch_reset(w);
    assert(w->entries[0].triggered == false);
    flashwatch_destroy(w);
    printf("  [PASS] test_reset\n");
}

static void test_get(void) {
    FlashWatch *w = flashwatch_create();
    flashwatch_add(w, "BOOT", 0x08000000, 0x4000);
    FlashWatchEntry *e = flashwatch_get(w, "BOOT");
    assert(e != NULL);
    assert(e->base == 0x08000000);
    assert(flashwatch_get(w, "MISSING") == NULL);
    flashwatch_destroy(w);
    printf("  [PASS] test_get\n");
}

int main(void) {
    printf("Running flashwatch tests...\n");
    test_create_destroy();
    test_add_remove();
    test_check_no_change();
    test_check_size_change();
    test_reset();
    test_get();
    printf("All flashwatch tests passed.\n");
    return 0;
}
