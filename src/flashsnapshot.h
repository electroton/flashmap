#ifndef FLASHSNAPSHOT_H
#define FLASHSNAPSHOT_H

#include <stdint.h>
#include <time.h>
#include "flashlayout.h"

/*
 * flashsnapshot — capture and restore named snapshots of flash layouts.
 * A snapshot records the layout state along with metadata (label, timestamp).
 */

#define FLASH_SNAPSHOT_LABEL_MAX 64
#define FLASH_SNAPSHOT_MAX       16

typedef struct {
    char            label[FLASH_SNAPSHOT_LABEL_MAX];
    time_t          timestamp;
    FlashLayout     layout;
} FlashSnapshot;

typedef struct {
    FlashSnapshot   entries[FLASH_SNAPSHOT_MAX];
    int             count;
} FlashSnapshotStore;

/* Initialise an empty snapshot store. */
void flash_snapshot_store_init(FlashSnapshotStore *store);

/* Capture the current layout into the store under the given label.
 * Returns 0 on success, -1 if the store is full or label is NULL. */
int flash_snapshot_capture(FlashSnapshotStore *store,
                           const FlashLayout  *layout,
                           const char         *label);

/* Retrieve a snapshot by label. Returns pointer into store or NULL. */
const FlashSnapshot *flash_snapshot_find(const FlashSnapshotStore *store,
                                         const char               *label);

/* Remove a snapshot by label. Returns 0 on success, -1 if not found. */
int flash_snapshot_remove(FlashSnapshotStore *store, const char *label);

/* List all snapshot labels to stdout. */
void flash_snapshot_list(const FlashSnapshotStore *store);

/* Restore a snapshot's layout into *out. Returns 0 on success, -1 if not found. */
int flash_snapshot_restore(const FlashSnapshotStore *store,
                           const char               *label,
                           FlashLayout              *out);

#endif /* FLASHSNAPSHOT_H */
