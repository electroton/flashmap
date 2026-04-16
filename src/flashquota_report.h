#ifndef FLASHQUOTA_REPORT_H
#define FLASHQUOTA_REPORT_H

#include "flashquota.h"
#include <stdio.h>

void flash_quota_report_print(const FlashQuota *quotas, int count, FILE *out);
void flash_quota_report_summary(const FlashQuota *quotas, int count, FILE *out);

#endif /* FLASHQUOTA_REPORT_H */
