/*
 * flashtrace_report.c - Reporting utilities for flash trace
 */
#include <stdio.h>
#include <string.h>
#include "flashtrace_report.h"

void flash_trace_report_full(const FlashTrace *trace) {
    if (!trace) return;
    printf("=== Flash Trace Full Report ===\n");
    flash_trace_print(trace);
    printf("\n");
    flash_trace_print_summary(trace);
    printf("==============================\n");
}

void flash_trace_report_by_region(const FlashTrace *trace) {
    if (!trace) return;

    /* Collect unique region names */
    char seen[FLASH_TRACE_MAX_EVENTS][FLASH_TRACE_NAME_LEN];
    int  seen_count = 0;

    for (int i = 0; i < trace->count; i++) {
        const char *name = trace->events[i].region_name;
        int found = 0;
        for (int j = 0; j < seen_count; j++) {
            if (strcmp(seen[j], name) == 0) { found = 1; break; }
        }
        if (!found && seen_count < FLASH_TRACE_MAX_EVENTS)
            strncpy(seen[seen_count++], name, FLASH_TRACE_NAME_LEN - 1);
    }

    printf("=== Trace by Region ===\n");
    printf("  %-20s  %5s  %5s  %5s  %5s\n",
           "Region", "READ", "WRITE", "ERASE", "VERIFY");
    printf("  %s\n", "------------------------------------------------");

    FlashTraceEvent buf[FLASH_TRACE_MAX_EVENTS];
    for (int r = 0; r < seen_count; r++) {
        int n = flash_trace_find_by_region(trace, seen[r], buf, FLASH_TRACE_MAX_EVENTS);
        int rd = 0, wr = 0, er = 0, vr = 0;
        for (int k = 0; k < n; k++) {
            switch (buf[k].type) {
                case TRACE_EVENT_READ:   rd++; break;
                case TRACE_EVENT_WRITE:  wr++; break;
                case TRACE_EVENT_ERASE:  er++; break;
                case TRACE_EVENT_VERIFY: vr++; break;
            }
        }
        printf("  %-20s  %5d  %5d  %5d  %5d\n", seen[r], rd, wr, er, vr);
    }
    printf("======================\n");
}

int flash_trace_report_csv(const FlashTrace *trace, const char *filepath) {
    if (!trace || !filepath) return -1;
    FILE *f = fopen(filepath, "w");
    if (!f) return -1;

    fprintf(f, "seq,type,address,size,region\n");
    for (int i = 0; i < trace->count; i++) {
        const FlashTraceEvent *ev = &trace->events[i];
        const char *type_str;
        switch (ev->type) {
            case TRACE_EVENT_READ:   type_str = "READ";   break;
            case TRACE_EVENT_WRITE:  type_str = "WRITE";  break;
            case TRACE_EVENT_ERASE:  type_str = "ERASE";  break;
            case TRACE_EVENT_VERIFY: type_str = "VERIFY"; break;
            default:                 type_str = "UNKNOWN"; break;
        }
        fprintf(f, "%d,%s,0x%08X,0x%08X,%s\n",
                ev->sequence, type_str, ev->address, ev->size, ev->region_name);
    }
    fclose(f);
    return 0;
}
