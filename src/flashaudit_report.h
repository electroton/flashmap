#ifndef FLASHAUDIT_REPORT_H
#define FLASHAUDIT_REPORT_H

#include "flashaudit.h"
#include <stdio.h>

/* Print a formatted audit report to the given stream */
void flash_audit_report_print(const AuditResult *result, FILE *out);

#endif /* FLASHAUDIT_REPORT_H */
