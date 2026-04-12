#include "flashhistory.h"

#include <string.h>
#include <stdio.h>

void flashhistory_init(FlashHistory *history)
{
    if (!history) return;
    memset(history, 0, sizeof(*history));
}

int flashhistory_record(FlashHistory *history,
                        const FlashLayout *layout,
                        const char *label)
{
    if (!history || !layout) return -1;
    if (history->count >= FLASHHISTORY_MAX_ENTRIES) return -1;

    FlashHistoryEntry *entry = &history->entries[history->count];
    entry->layout    = *layout;
    entry->timestamp = time(NULL);

    if (label && label[0] != '\0') {
        strncpy(entry->label, label, FLASHHISTORY_LABEL_LEN - 1);
        entry->label[FLASHHISTORY_LABEL_LEN - 1] = '\0';
    } else {
        snprintf(entry->label, FLASHHISTORY_LABEL_LEN,
                 "snapshot_%zu", history->count);
    }

    history->count++;
    return 0;
}

const FlashHistoryEntry *flashhistory_get(const FlashHistory *history,
                                          size_t index)
{
    if (!history || index >= history->count) return NULL;
    return &history->entries[index];
}

const FlashHistoryEntry *flashhistory_find_by_label(
        const FlashHistory *history, const char *label)
{
    if (!history || !label) return NULL;

    /* Iterate in reverse to return the most recent match */
    for (size_t i = history->count; i > 0; i--) {
        const FlashHistoryEntry *e = &history->entries[i - 1];
        if (strncmp(e->label, label, FLASHHISTORY_LABEL_LEN) == 0) {
            return e;
        }
    }
    return NULL;
}

size_t flashhistory_count(const FlashHistory *history)
{
    if (!history) return 0;
    return history->count;
}

void flashhistory_clear(FlashHistory *history)
{
    if (!history) return;
    memset(history, 0, sizeof(*history));
}
