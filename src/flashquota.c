#include "flashquota.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashQuota flash_quota_create(const char *region_name, uint32_t max_bytes) {
    FlashQuota q;
    memset(&q, 0, sizeof(q));
    strncpy(q.region_name, region_name, FLASH_QUOTA_NAME_MAX - 1);
    q.max_bytes = max_bytes;
    q.used_bytes = 0;
    return q;
}

bool flash_quota_check(const FlashQuota *q) {
    if (!q) return false;
    return q->used_bytes <= q->max_bytes;
}

FlashQuotaStatus flash_quota_status(const FlashQuota *q) {
    if (!q) return FLASH_QUOTA_ERROR;
    if (q->used_bytes > q->max_bytes) return FLASH_QUOTA_EXCEEDED;
    if (q->used_bytes == q->max_bytes) return FLASH_QUOTA_FULL;
    uint32_t pct = (uint32_t)(((uint64_t)q->used_bytes * 100) / q->max_bytes);
    if (pct >= 90) return FLASH_QUOTA_WARNING;
    return FLASH_QUOTA_OK;
}

void flash_quota_update(FlashQuota *q, uint32_t used_bytes) {
    if (!q) return;
    q->used_bytes = used_bytes;
}

uint32_t flash_quota_remaining(const FlashQuota *q) {
    if (!q || q->used_bytes >= q->max_bytes) return 0;
    return q->max_bytes - q->used_bytes;
}

float flash_quota_usage_percent(const FlashQuota *q) {
    if (!q || q->max_bytes == 0) return 0.0f;
    return (float)q->used_bytes / (float)q->max_bytes * 100.0f;
}

void flash_quota_apply_to_layout(FlashQuota *quotas, int count, const FlashLayout *layout) {
    if (!quotas || !layout) return;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < layout->count; j++) {
            if (strcmp(quotas[i].region_name, layout->regions[j].name) == 0) {
                flash_quota_update(&quotas[i], layout->regions[j].size);
                break;
            }
        }
    }
}
