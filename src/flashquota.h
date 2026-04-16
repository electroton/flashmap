#ifndef FLASHQUOTA_H
#define FLASHQUOTA_H

#include <stdint.h>
#include <stdbool.h>
#include "flashlayout.h"

#define FLASH_QUOTA_NAME_MAX 64

typedef enum {
    FLASH_QUOTA_OK,
    FLASH_QUOTA_WARNING,
    FLASH_QUOTA_FULL,
    FLASH_QUOTA_EXCEEDED,
    FLASH_QUOTA_ERROR
} FlashQuotaStatus;

typedef struct {
    char     region_name[FLASH_QUOTA_NAME_MAX];
    uint32_t max_bytes;
    uint32_t used_bytes;
} FlashQuota;

FlashQuota       flash_quota_create(const char *region_name, uint32_t max_bytes);
bool             flash_quota_check(const FlashQuota *q);
FlashQuotaStatus flash_quota_status(const FlashQuota *q);
void             flash_quota_update(FlashQuota *q, uint32_t used_bytes);
uint32_t         flash_quota_remaining(const FlashQuota *q);
float            flash_quota_usage_percent(const FlashQuota *q);
void             flash_quota_apply_to_layout(FlashQuota *quotas, int count, const FlashLayout *layout);

#endif /* FLASHQUOTA_H */
