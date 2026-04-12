#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/flashwatch.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* Helper: build a simple layout with up to 4 regions */
static FlashLayout make_layout(const char *names[], uint32_t addrs[],
                               uint32_t sizes[], int count) {
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    for (int i = 0; i < count; i++) {
        FlashRegion r;
        memset(&r, 0, sizeof(r));
        strncpy(r.name, names[i], sizeof(r.name) - 1);
        r.address = addrs[i];
        r.size    = sizes[i];
        flashlayout_add(&l, &r);
    }
    return l;
}

static void test_no_changes(void) {
    const char *names[] = {"boot", "app"};
    uint32_t    addrs[] = {0x0000, 0x1000};
    uint32_t    sizes[] = {0x1000, 0x8000};

    FlashLayout old = make_layout(names, addrs, sizes, 2);
    FlashLayout new = make_layout(names, addrs, sizes, 2);
    FlashWatchResult result;

    int rc = flashwatch_compare(&old, &new, &result);
    assert(rc == 0);
    assert(result.count == 0);

    flashwatch_free(&result);
    flashlayout_free(&old);
    flashlayout_free(&new);
    printf("PASS: test_no_changes\n");
}

static void test_region_added(void) {
    const char *old_names[] = {"boot"};
    uint32_t    old_addrs[] = {0x0000};
    uint32_t    old_sizes[] = {0x1000};

    const char *new_names[] = {"boot", "app"};
    uint32_t    new_addrs[] = {0x0000, 0x1000};
    uint32_t    new_sizes[] = {0x1000, 0x8000};

    FlashLayout old = make_layout(old_names, old_addrs, old_sizes, 1);
    FlashLayout new = make_layout(new_names, new_addrs, new_sizes, 2);
    FlashWatchResult result;

    int rc = flashwatch_compare(&old, &new, &result);
    assert(rc == 0);
    assert(result.count == 1);
    assert(result.events[0].type == WATCH_ADDED);
    assert(strcmp(result.events[0].name, "app") == 0);

    flashwatch_free(&result);
    flashlayout_free(&old);
    flashlayout_free(&new);
    printf("PASS: test_region_added\n");
}

static void test_region_removed(void) {
    const char *old_names[] = {"boot", "app"};
    uint32_t    old_addrs[] = {0x0000, 0x1000};
    uint32_t    old_sizes[] = {0x1000, 0x8000};

    const char *new_names[] = {"boot"};
    uint32_t    new_addrs[] = {0x0000};
    uint32_t    new_sizes[] = {0x1000};

    FlashLayout old = make_layout(old_names, old_addrs, old_sizes, 2);
    FlashLayout new = make_layout(new_names, new_addrs, new_sizes, 1);
    FlashWatchResult result;

    int rc = flashwatch_compare(&old, &new, &result);
    assert(rc == 0);
    assert(result.count == 1);
    assert(result.events[0].type == WATCH_REMOVED);
    assert(strcmp(result.events[0].name, "app") == 0);

    flashwatch_free(&result);
    flashlayout_free(&old);
    flashlayout_free(&new);
    printf("PASS: test_region_removed\n");
}

static void test_region_resized(void) {
    const char *names[]     = {"boot"};
    uint32_t    addrs[]     = {0x0000};
    uint32_t    old_sizes[] = {0x1000};
    uint32_t    new_sizes[] = {0x2000};

    FlashLayout old = make_layout(names, addrs, old_sizes, 1);
    FlashLayout new = make_layout(names, addrs, new_sizes, 1);
    FlashWatchResult result;

    int rc = flashwatch_compare(&old, &new, &result);
    assert(rc == 0);
    assert(result.count == 1);
    assert(result.events[0].type == WATCH_RESIZED);
    assert(result.events[0].old_size == 0x1000);
    assert(result.events[0].new_size == 0x2000);

    flashwatch_free(&result);
    flashlayout_free(&old);
    flashlayout_free(&new);
    printf("PASS: test_region_resized\n");
}

static void test_region_moved(void) {
    const char *names[]     = {"app"};
    uint32_t    old_addrs[] = {0x1000};
    uint32_t    new_addrs[] = {0x2000};
    uint32_t    sizes[]     = {0x4000};

    FlashLayout old = make_layout(names, old_addrs, sizes, 1);
    FlashLayout new = make_layout(names, new_addrs, sizes, 1);
    FlashWatchResult result;

    int rc = flashwatch_compare(&old, &new, &result);
    assert(rc == 0);
    assert(result.count == 1);
    assert(result.events[0].type == WATCH_MOVED);
    assert(result.events[0].old_address == 0x1000);
    assert(result.events[0].new_address == 0x2000);

    flashwatch_free(&result);
    flashlayout_free(&old);
    flashlayout_free(&new);
    printf("PASS: test_region_moved\n");
}

int main(void) {
    test_no_changes();
    test_region_added();
    test_region_removed();
    test_region_resized();
    test_region_moved();
    printf("All flashwatch tests passed.\n");
    return 0;
}
