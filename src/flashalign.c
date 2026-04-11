#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flashalign.h"

bool flashalign_is_aligned(uint32_t address, uint32_t alignment) {
    if (alignment == 0) return true;
    return (address % alignment) == 0;
}

static void build_issue(AlignIssue *issue, const FlashRegion *region,
                        uint32_t alignment, bool addr_bad, bool size_bad) {
    strncpy(issue->region_name, region->name, sizeof(issue->region_name) - 1);
    issue->region_name[sizeof(issue->region_name) - 1] = '\0';
    issue->address = region->address;
    issue->size = region->size;
    issue->required_alignment = alignment;

    if (addr_bad && size_bad) {
        _alignment = region->address % alignment;
        issue->severity = ALIGN_ERROR;
        snprintf(issue->message, sizeof(issue->message),
                 "Address 0x%08X and size 0x%X are not aligned to 0x%X",
                 region->address, region->size, alignment);
    } else if (addr_bad) {
        issue->actual_alignment = region->address % alignment;
        issue->severity = ALIGN_ERROR;
        snprintf(issue->message, sizeof(issue->message),
                 "Address 0x%08X is not aligned to 0x%X",
                 region->address, alignment);
    } else {
        issue->actual_alignment = region->size % alignment;
        issue->severity = ALIGN_WARNING;
        snprintf(issue->message, sizeof(issue->message),
                 "Size 0x%X is not aligned to 0x%X",
                 region->size, alignment);
    }
}

AlignReport flashalign_check(const FlashLayout *layout, uint32_t alignment) {
    AlignReport report = {0};
    if (!layout || alignment == 0) return report;

    /* Allocate worst-case: two issues per region */
    report.issues = calloc(layout->region_count * 2, sizeof(AlignIssue));
    if (!report.issues) return report;

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        bool addr_bad = !flashalign_is_aligned(r->address, alignment);
        bool size_bad = !flashalign_is_aligned(r->size, alignment);

        if (addr_bad || size_bad) {
            AlignIssue *issue = &report.issues[report.issue_count++];
            build_issue(issue, r, alignment, addr_bad, size_bad);
            if (issue->severity == ALIGN_ERROR)
                report.error_count++;
            else
                report.warning_count++;
        }
    }
    return report;
}

void flashalign_print_report(const AlignReport *report) {
    if (!report) return;
    if (report->issue_count == 0) {
        printf("Alignment check passed: no issues found.\n");
        return;
    }
    printf("Alignment Report: %d error(s), %d warning(s)\n",
           report->error_count, report->warning_count);
    for (int i = 0; i < report->issue_count; i++) {
        const AlignIssue *iss = &report->issues[i];
        const char *sev = (iss->severity == ALIGN_ERROR) ? "ERROR" : "WARNING";
        printf("  [%s] %s: %s\n", sev, iss->region_name, iss->message);
    }
}

void flashalign_free_report(AlignReport *report) {
    if (!report) return;
    free(report->issues);
    report->issues = NULL;
    report->issue_count = 0;
    report->error_count = 0;
    report->warning_count = 0;
}
