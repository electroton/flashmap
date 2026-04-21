/*
 * flashresize_report.c
 * Reporting utilities for flash region resize operations.
 *
 * Formats and prints resize results, including before/after sizes,
 * delta values, and any warnings about capacity limits.
 */

#include "flashresize_report.h"
#include "flashresize.h"
#include <stdio.h>
#include <stddef.h>

/* Print a single resize result entry */
static void print_resize_entry(const FlashResizeResult *result)
{
    if (!result) return;

    printf("  Region : %s\n", result->region_name ? result->region_name : "<unnamed>");
    printf("  Before : 0x%08X (%u bytes)\n", result->original_size, result->original_size);
    printf("  After  : 0x%08X (%u bytes)\n", result->new_size, result->new_size);

    int delta = (int)result->new_size - (int)result->original_size;
    if (delta > 0) {
        printf("  Delta  : +%d bytes (grown)\n", delta);
    } else if (delta < 0) {
        printf("  Delta  : %d bytes (shrunk)\n", delta);
    } else {
        printf("  Delta  : 0 bytes (unchanged)\n");
    }

    if (result->clamped) {
        printf("  WARNING: Size was clamped to fit flash boundary.\n");
    }
    if (result->overflow) {
        printf("  ERROR:   Resize exceeds available flash capacity!\n");
    }
}

/* Print a full resize report for all results in a set */
void flash_resize_report_print(const FlashResizeReport *report)
{
    if (!report) return;

    printf("=== Flash Resize Report ===\n");
    printf("Total regions resized: %zu\n\n", report->count);

    for (size_t i = 0; i < report->count; i++) {
        printf("[%zu]\n", i + 1);
        print_resize_entry(&report->results[i]);
        printf("\n");
    }

    /* Summary line */
    size_t warnings = 0;
    size_t errors = 0;
    for (size_t i = 0; i < report->count; i++) {
        if (report->results[i].clamped)  warnings++;
        if (report->results[i].overflow) errors++;
    }

    printf("--- Summary ---\n");
    printf("  Warnings : %zu\n", warnings);
    printf("  Errors   : %zu\n", errors);
    printf("===========================\n");
}

/* Print a compact one-line summary per resize result */
void flash_resize_report_print_compact(const FlashResizeReport *report)
{
    if (!report) return;

    printf("Flash Resize Summary (%zu region(s)):\n", report->count);
    for (size_t i = 0; i < report->count; i++) {
        const FlashResizeResult *r = &report->results[i];
        int delta = (int)r->new_size - (int)r->original_size;
        printf("  %-24s %6u -> %6u bytes (%+d)%s%s\n",
            r->region_name ? r->region_name : "<unnamed>",
            r->original_size,
            r->new_size,
            delta,
            r->clamped  ? " [CLAMPED]"  : "",
            r->overflow ? " [OVERFLOW]" : "");
    }
}
