#ifndef FLASHAUDIT_H
#define FLASHAUDIT_H

#include "flashregion.h"
#include "flashlayout.h"
#include <stdint.h>
#include <stdbool.h>

/* Audit severity levels */
typedef enum {
    AUDIT_INFO    = 0,
    AUDIT_WARNING = 1,
    AUDIT_ERROR   = 2
} AuditSeverity;

/* Audit rule types */
typedef enum {
    AUDIT_RULE_MIN_SIZE,
    AUDIT_RULE_MAX_SIZE,
    AUDIT_RULE_ALIGNMENT,
    AUDIT_RULE_NO_OVERLAP,
    AUDIT_RULE_FILL_THRESHOLD
} AuditRuleType;

typedef struct {
    AuditRuleType   type;
    char            region_name[64]; /* empty = apply to all */
    uint32_t        threshold;       /* bytes or percent depending on rule */
    AuditSeverity   severity;
} AuditRule;

typedef struct {
    AuditSeverity   severity;
    AuditRuleType   rule_type;
    char            region_name[64];
    char            message[256];
} AuditFinding;

typedef struct {
    AuditFinding   *findings;
    int             count;
    int             capacity;
} AuditResult;

/* Run all rules against a layout */
AuditResult *flash_audit_run(const FlashLayout *layout,
                             const AuditRule   *rules,
                             int                rule_count);

/* Query helpers */
int  flash_audit_count_by_severity(const AuditResult *result, AuditSeverity sev);
bool flash_audit_has_errors(const AuditResult *result);

void flash_audit_result_free(AuditResult *result);

#endif /* FLASHAUDIT_H */
