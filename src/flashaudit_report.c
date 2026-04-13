/**
 * flashaudit_report.c
 *
 * Report generation for flash memory audit results.
 * Formats and prints audit findings including violations,
 * warnings, and compliance summaries.
 */

#include "flashaudit_report.h"
#include "flashaudit.h"
#include <stdio.h>
#include <string.h>

/* Severity label strings */
static const char *severity_label(FlashAuditSeverity severity) {
    switch (severity) {
        case AUDIT_SEVERITY_INFO:    return "INFO";
        case AUDIT_SEVERITY_WARNING: return "WARNING";
        case AUDIT_SEVERITY_ERROR:   return "ERROR";
        case AUDIT_SEVERITY_FATAL:   return "FATAL";
        default:                     return "UNKNOWN";
    }
}

/**
 * Print a single audit finding to the given file stream.
 *
 * @param out     Output stream
 * @param finding Pointer to the audit finding to print
 */
void flashaudit_report_finding(FILE *out, const FlashAuditFinding *finding) {
    if (!out || !finding) return;

    fprintf(out, "  [%-7s] %s\n",
            severity_label(finding->severity),
            finding->message);

    if (finding->region_name[0] != '\0') {
        fprintf(out, "             Region : %s\n", finding->region_name);
    }

    if (finding->address != 0 || finding->size != 0) {
        fprintf(out, "             Addr   : 0x%08X  Size: 0x%08X (%u bytes)\n",
                finding->address, finding->size, finding->size);
    }
}

/**
 * Print a full audit report to the given file stream.
 *
 * Includes a header, all findings grouped by severity, and a
 * summary line with total counts.
 *
 * @param out    Output stream
 * @param report Pointer to the completed audit report
 */
void flashaudit_report_print(FILE *out, const FlashAuditReport *report) {
    if (!out || !report) return;

    fprintf(out, "\n=== Flash Audit Report ===\n");
    fprintf(out, "Rule set : %s\n", report->ruleset_name[0] ? report->ruleset_name : "(default)");
    fprintf(out, "Findings : %u\n\n", report->finding_count);

    if (report->finding_count == 0) {
        fprintf(out, "  No issues found. All checks passed.\n");
    } else {
        /* Print fatal and error findings first */
        for (unsigned int i = 0; i < report->finding_count; i++) {
            const FlashAuditFinding *f = &report->findings[i];
            if (f->severity == AUDIT_SEVERITY_FATAL ||
                f->severity == AUDIT_SEVERITY_ERROR) {
                flashaudit_report_finding(out, f);
            }
        }
        /* Then warnings and info */
        for (unsigned int i = 0; i < report->finding_count; i++) {
            const FlashAuditFinding *f = &report->findings[i];
            if (f->severity == AUDIT_SEVERITY_WARNING ||
                f->severity == AUDIT_SEVERITY_INFO) {
                flashaudit_report_finding(out, f);
            }
        }
    }

    fprintf(out, "\nSummary: %u fatal, %u error, %u warning, %u info\n",
            report->count_fatal,
            report->count_error,
            report->count_warning,
            report->count_info);

    fprintf(out, "Result  : %s\n",
            (report->count_fatal == 0 && report->count_error == 0)
                ? "PASS" : "FAIL");

    fprintf(out, "==========================\n\n");
}
