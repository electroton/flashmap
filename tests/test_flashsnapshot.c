#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashsnapshot.h"
#include "../src/flashsnapshot_report.h"

static FlashLayout make_layout(int region_count)
{
    FlashLayout l;
    memset(&l, 0, sizeof(l));
    l.region_count = region_count;
    for (int i = 0; i < region_count; i++) {
        l.regions[i].start = (uint32_t)(i * 0x1000);
        l.regions[i].size  = 0x1000;
        snprintf(l.regions[i].name, sizeof(l.regions[i].name), "region%d", i);
    }
    return l;
}

static void test_init(void)
{
    FlashSnapshotStore store;
    flash_snapshot_store_init(&store);
    assert(store.count == 0);
    printf("PASS test_init\n");
}

static void test_capture_and_find(void)
{
    FlashSnapshotStore store;
    flash_snapshot_store_init(&store);

    FlashLayout l = make_layout(3);
    int rc = flash_snapshot_capture(&store, &l, "baseline");
    assert(rc == 0);
    assert(store.count == 1);

    const FlashSnapshot *s = flash_snapshot_find(&store, "baseline");
    assert(s != NULL);
    assert(strcmp(s->label, "baseline") == 0);
    assert(s->layout.region_count == 3);

    assert(flash_snapshot_find(&store, "missing") == NULL);
    printf("PASS test_capture_and_find\n");
}

static void test_remove(void)
{
    FlashSnapshotStore store;
    flash_snapshot_store_init(&store);

    FlashLayout l = make_layout(2);
    flash_snapshot_capture(&store, &l, "snap_a");
    flash_snapshot_capture(&store, &l, "snap_b");
    assert(store.count == 2);

    int rc = flash_snapshot_remove(&store, "snap_a");
    assert(rc == 0);
    assert(store.count == 1);
    assert(flash_snapshot_find(&store, "snap_a") == NULL);
    assert(flash_snapshot_find(&store, "snap_b") != NULL);

    assert(flash_snapshot_remove(&store, "nonexistent") == -1);
    printf("PASS test_remove\n");
}

static void test_restore(void)
{
    FlashSnapshotStore store;
    flash_snapshot_store_init(&store);

    FlashLayout orig = make_layout(4);
    flash_snapshot_capture(&store, &orig, "v1");

    FlashLayout restored;
    memset(&restored, 0, sizeof(restored));
    int rc = flash_snapshot_restore(&store, "v1", &restored);
    assert(rc == 0);
    assert(restored.region_count == 4);
    assert(restored.regions[2].start == orig.regions[2].start);

    assert(flash_snapshot_restore(&store, "vX", &restored) == -1);
    printf("PASS test_restore\n");
}

static void test_store_full(void)
{
    FlashSnapshotStore store;
    flash_snapshot_store_init(&store);
    FlashLayout l = make_layout(1);
    char label[32];
    for (int i = 0; i < FLASH_SNAPSHOT_MAX; i++) {
        snprintf(label, sizeof(label), "snap_%d", i);
        assert(flash_snapshot_capture(&store, &l, label) == 0);
    }
    assert(flash_snapshot_capture(&store, &l, "overflow") == -1);
    printf("PASS test_store_full\n");
}

static void test_report(void)
{
    FlashSnapshotStore store;
    flash_snapshot_store_init(&store);
    FlashLayout l = make_layout(2);
    flash_snapshot_capture(&store, &l, "report_test");
    flash_snapshot_report_all(&store);
    const FlashSnapshot *s = flash_snapshot_find(&store, "report_test");
    flash_snapshot_report(s);
    printf("PASS test_report\n");
}

int main(void)
{
    test_init();
    test_capture_and_find();
    test_remove();
    test_restore();
    test_store_full();
    test_report();
    printf("All flashsnapshot tests passed.\n");
    return 0;
}
