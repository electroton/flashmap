#ifndef FLASHQUOTA_H
#define FLASHQUOTA_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

/* Quota enforcement modes */
typedef enum {
    QUOTA_MODE_WARN  = 0,  /* Emit warning when quota exceeded */
    QUOTA_MODE_ERROR = 1   /* Treat quota violation as an error */
} FlashQuotaMode;

/* A quota rule applied to a named region */
typedef struct {
    char        region_name[64]; /* Region the quota applies to */
    uint32_t    max_used;        /* Maximum allowed used bytes   */
    float       max_percent;     /* Maximum allowed usage (0-100)*/
    FlashQuotaMode mode;
} FlashQuotaRule;

/* Result of checking a single quota rule */
typedef struct {
    FlashQuotaRule rule;
    uint32_t       actual_used;
    float          actual_percent;
    int            violated;     /* 1 if quota exceeded, 0 otherwise */
} FlashQuotaResult;

/* Collection of results */
typedef struct {
    FlashQuotaResult *results;
    size_t            count;
    int               any_error;  /* 1 if any ERROR-mode rule was violated */
} FlashQuotaReport;

/*
 * Check all quota rules against the given regions.
 * Caller must free the report with flash_quota_report_free().
 */
FlashQuotaReport flash_quota_check(const FlashQuotaRule *rules,
                                   size_t                rule_count,
                                   const FlashRegion    *regions,
                                   size_t                region_count);

void flash_quota_report_print(const FlashQuotaReport *report);
void flash_quota_report_free(FlashQuotaReport *report);

#endif /* FLASHQUOTA_H */
