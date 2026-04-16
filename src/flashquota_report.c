#include "flashquota_report.h"
#include <stdio.h>

static const char *status_str(FlashQuotaStatus s) {
    switch (s) {
        case FLASH_QUOTA_OK:       return "OK";
        case FLASH_QUOTA_WARNING:  return "WARNING";
        case FLASH_QUOTA_FULL:     return "FULL";
        case FLASH_QUOTA_EXCEEDED: return "EXCEEDED";
        default:                   return "ERROR";
    }
}

void flash_quota_report_print(const FlashQuota *quotas, int count, FILE *out) {
    if (!quotas || !out) return;
    fprintf(out, "%-24s %10s %10s %10s %8s %10s\n",
            "Region", "Max(B)", "Used(B)", "Free(B)", "Use%", "Status");
    fprintf(out, "%-24s %10s %10s %10s %8s %10s\n",
            "------------------------",
            "----------", "----------", "----------",
            "--------", "----------");
    for (int i = 0; i < count; i++) {
        const FlashQuota *q = &quotas[i];
        FlashQuotaStatus st = flash_quota_status(q);
        fprintf(out, "%-24s %10u %10u %10u %7.1f%% %10s\n",
                q->region_name,
                q->max_bytes,
                q->used_bytes,
                flash_quota_remaining(q),
                flash_quota_usage_percent(q),
                status_str(st));
    }
}

void flash_quota_report_summary(const FlashQuota *quotas, int count, FILE *out) {
    if (!quotas || !out) return;
    int ok = 0, warn = 0, exceeded = 0;
    for (int i = 0; i < count; i++) {
        FlashQuotaStatus st = flash_quota_status(&quotas[i]);
        if (st == FLASH_QUOTA_OK || st == FLASH_QUOTA_FULL) ok++;
        else if (st == FLASH_QUOTA_WARNING) warn++;
        else if (st == FLASH_QUOTA_EXCEEDED) exceeded++;
    }
    fprintf(out, "Quota Summary: %d region(s) — OK: %d, Warning: %d, Exceeded: %d\n",
            count, ok, warn, exceeded);
}
