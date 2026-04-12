#include <stdio.h>
#include <time.h>
#include "flashsnapshot_report.h"

void flash_snapshot_report(const FlashSnapshot *snap)
{
    if (!snap) return;

    char buf[32];
    struct tm *tm_info = localtime(&snap->timestamp);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("Snapshot : %s\n", snap->label);
    printf("Captured : %s\n", buf);
    printf("Regions  : %d\n", snap->layout.region_count);

    uint32_t total = 0;
    for (int i = 0; i < snap->layout.region_count; i++)
        total += snap->layout.regions[i].size;
    printf("Total    : %u bytes\n", total);
    printf("\n");
}

void flash_snapshot_report_all(const FlashSnapshotStore *store)
{
    if (!store) return;
    printf("=== Flash Snapshot Store (%d/%d) ===\n",
           store->count, FLASH_SNAPSHOT_MAX);
    if (store->count == 0) {
        printf("  (empty)\n");
        return;
    }
    printf("%-4s %-32s %-20s %s\n",
           "#", "Label", "Captured", "Regions");
    printf("%-4s %-32s %-20s %s\n",
           "---", "------------------------------",
           "-------------------", "-------");
    for (int i = 0; i < store->count; i++) {
        const FlashSnapshot *s = &store->entries[i];
        char buf[32];
        struct tm *tm_info = localtime(&s->timestamp);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("%-4d %-32s %-20s %d\n",
               i, s->label, buf, s->layout.region_count);
    }
}
