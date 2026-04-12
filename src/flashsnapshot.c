#include <stdio.h>
#include <string.h>
#include <time.h>
#include "flashsnapshot.h"

void flash_snapshot_store_init(FlashSnapshotStore *store)
{
    if (!store) return;
    memset(store, 0, sizeof(*store));
}

int flash_snapshot_capture(FlashSnapshotStore *store,
                           const FlashLayout  *layout,
                           const char         *label)
{
    if (!store || !layout || !label) return -1;
    if (store->count >= FLASH_SNAPSHOT_MAX) return -1;

    FlashSnapshot *snap = &store->entries[store->count];
    strncpy(snap->label, label, FLASH_SNAPSHOT_LABEL_MAX - 1);
    snap->label[FLASH_SNAPSHOT_LABEL_MAX - 1] = '\0';
    snap->timestamp = time(NULL);
    snap->layout    = *layout;   /* shallow copy — FlashLayout is value-type */

    store->count++;
    return 0;
}

const FlashSnapshot *flash_snapshot_find(const FlashSnapshotStore *store,
                                         const char               *label)
{
    if (!store || !label) return NULL;
    for (int i = 0; i < store->count; i++) {
        if (strncmp(store->entries[i].label, label,
                    FLASH_SNAPSHOT_LABEL_MAX) == 0) {
            return &store->entries[i];
        }
    }
    return NULL;
}

int flash_snapshot_remove(FlashSnapshotStore *store, const char *label)
{
    if (!store || !label) return -1;
    for (int i = 0; i < store->count; i++) {
        if (strncmp(store->entries[i].label, label,
                    FLASH_SNAPSHOT_LABEL_MAX) == 0) {
            /* Shift remaining entries down */
            for (int j = i; j < store->count - 1; j++)
                store->entries[j] = store->entries[j + 1];
            store->count--;
            return 0;
        }
    }
    return -1;
}

void flash_snapshot_list(const FlashSnapshotStore *store)
{
    if (!store) return;
    if (store->count == 0) {
        printf("(no snapshots)\n");
        return;
    }
    for (int i = 0; i < store->count; i++) {
        char buf[32];
        struct tm *tm_info = localtime(&store->entries[i].timestamp);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("[%d] %-32s  %s\n", i, store->entries[i].label, buf);
    }
}

int flash_snapshot_restore(const FlashSnapshotStore *store,
                           const char               *label,
                           FlashLayout              *out)
{
    const FlashSnapshot *snap = flash_snapshot_find(store, label);
    if (!snap || !out) return -1;
    *out = snap->layout;
    return 0;
}
