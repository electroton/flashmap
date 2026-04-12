#ifndef FLASHSNAPSHOT_REPORT_H
#define FLASHSNAPSHOT_REPORT_H

#include "flashsnapshot.h"

/*
 * Print a human-readable report of a single snapshot to stdout.
 */
void flash_snapshot_report(const FlashSnapshot *snap);

/*
 * Print a summary table of all snapshots in the store.
 */
void flash_snapshot_report_all(const FlashSnapshotStore *store);

#endif /* FLASHSNAPSHOT_REPORT_H */
