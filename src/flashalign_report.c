#include "flashalign_report.h"
#include <stdio.h>
#include <stdint.h>

void flashalign_report_violations(const FlashAlignResult *result, FILE *stream)
{
    if (!result || !stream) return;

    if (result->violation_count == 0) {
        fprintf(stream, "[flashalign] No alignment violations found.\n");
        return;
    }

    fprintf(stream, "[flashalign] %u alignment violation(s) detected:\n",
            result->violation_count);

    for (uint32_t i = 0; i < result->violation_count; i++) {
        const FlashAlignViolation *v = &result->violations[i];
        fprintf(stream,
                "  [%u] Region '%s': start=0x%08X size=0x%08X "
                "(expected alignment: 0x%X)\n",
                i + 1,
                v->region_name ? v->region_name : "<unnamed>",
                v->start_address,
                v->size,
                v->required_alignment);
    }
}

void flashalign_report_detailed(const FlashAlignResult *result,
                                 uint32_t alignment,
                                 FILE *stream)
{
    if (!result || !stream) return;

    fprintf(stream, "[flashalign] Detailed Alignment Report (align=0x%X)\n",
            alignment);
    fprintf(stream, "  Total regions checked : %u\n", result->regions_checked);
    fprintf(stream, "  Violations            : %u\n", result->violation_count);
    fprintf(stream, "  Compliant regions     : %u\n",
            result->regions_checked - result->violation_count);

    if (result->violation_count > 0) {
        fprintf(stream, "\n  Violations:\n");
        flashalign_report_violations(result, stream);
    }
}

void flashalign_report_summary(const FlashAlignResult *result, FILE *stream)
{
    if (!result || !stream) return;

    if (result->violation_count == 0) {
        fprintf(stream,
                "flashalign: OK (%u region(s) checked, 0 violations)\n",
                result->regions_checked);
    } else {
        fprintf(stream,
                "flashalign: FAIL (%u region(s) checked, %u violation(s))\n",
                result->regions_checked,
                result->violation_count);
    }
}
