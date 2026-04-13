#include "flashaudit.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static AuditResult *result_new(void) {
    AuditResult *r = calloc(1, sizeof(AuditResult));
    r->capacity = 16;
    r->findings = calloc(r->capacity, sizeof(AuditFinding));
    return r;
}

static void result_push(AuditResult *r, AuditSeverity sev, AuditRuleType type,
                        const char *name, const char *msg) {
    if (r->count >= r->capacity) {
        r->capacity *= 2;
        r->findings = realloc(r->findings, r->capacity * sizeof(AuditFinding));
    }
    AuditFinding *f = &r->findings[r->count++];
    f->severity  = sev;
    f->rule_type = type;
    strncpy(f->region_name, name ? name : "", sizeof(f->region_name) - 1);
    strncpy(f->message, msg, sizeof(f->message) - 1);
}

static bool rule_applies(const AuditRule *rule, const char *name) {
    return rule->region_name[0] == '\0' ||
           strcmp(rule->region_name, name) == 0;
}

static void apply_rule(const FlashLayout *layout, const AuditRule *rule,
                       AuditResult *result) {
    char msg[256];
    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *reg = &layout->regions[i];
        if (!rule_applies(rule, reg->name)) continue;

        switch (rule->type) {
            case AUDIT_RULE_MIN_SIZE:
                if (reg->size < rule->threshold) {
                    snprintf(msg, sizeof(msg),
                             "Region '%s' size 0x%X below minimum 0x%X",
                             reg->name, reg->size, rule->threshold);
                    result_push(result, rule->severity, rule->type, reg->name, msg);
                }
                break;
            case AUDIT_RULE_MAX_SIZE:
                if (reg->size > rule->threshold) {
                    snprintf(msg, sizeof(msg),
                             "Region '%s' size 0x%X exceeds maximum 0x%X",
                             reg->name, reg->size, rule->threshold);
                    result_push(result, rule->severity, rule->type, reg->name, msg);
                }
                break;
            case AUDIT_RULE_ALIGNMENT:
                if (rule->threshold > 0 && (reg->start % rule->threshold) != 0) {
                    snprintf(msg, sizeof(msg),
                             "Region '%s' start 0x%X not aligned to 0x%X",
                             reg->name, reg->start, rule->threshold);
                    result_push(result, rule->severity, rule->type, reg->name, msg);
                }
                break;
            case AUDIT_RULE_FILL_THRESHOLD:
                if (reg->size > 0) {
                    uint32_t pct = (reg->used * 100) / reg->size;
                    if (pct > rule->threshold) {
                        snprintf(msg, sizeof(msg),
                                 "Region '%s' fill %u%% exceeds threshold %u%%",
                                 reg->name, pct, rule->threshold);
                        result_push(result, rule->severity, rule->type, reg->name, msg);
                    }
                }
                break;
            default:
                break;
        }
    }
}

AuditResult *flash_audit_run(const FlashLayout *layout,
                             const AuditRule   *rules,
                             int                rule_count) {
    AuditResult *result = result_new();
    for (int i = 0; i < rule_count; i++)
        apply_rule(layout, &rules[i], result);
    return result;
}

int flash_audit_count_by_severity(const AuditResult *result, AuditSeverity sev) {
    int n = 0;
    for (int i = 0; i < result->count; i++)
        if (result->findings[i].severity == sev) n++;
    return n;
}

bool flash_audit_has_errors(const AuditResult *result) {
    return flash_audit_count_by_severity(result, AUDIT_ERROR) > 0;
}

void flash_audit_result_free(AuditResult *result) {
    if (!result) return;
    free(result->findings);
    free(result);
}
