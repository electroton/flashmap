/*
 * flashtrace.c - Flash memory region access tracing implementation
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "flashtrace.h"

static const char *event_type_str(FlashTraceEventType type) {
    switch (type) {
        case TRACE_EVENT_READ:   return "READ";
        case TRACE_EVENT_WRITE:  return "WRITE";
        case TRACE_EVENT_ERASE:  return "ERASE";
        case TRACE_EVENT_VERIFY: return "VERIFY";
        default:                 return "UNKNOWN";
    }
}

void flash_trace_init(FlashTrace *trace) {
    if (!trace) return;
    memset(trace, 0, sizeof(*trace));
    trace->enabled = 1;
}

void flash_trace_reset(FlashTrace *trace) {
    if (!trace) return;
    trace->count    = 0;
    trace->next_seq = 0;
    memset(trace->events, 0, sizeof(trace->events));
}

void flash_trace_enable(FlashTrace *trace) {
    if (trace) trace->enabled = 1;
}

void flash_trace_disable(FlashTrace *trace) {
    if (trace) trace->enabled = 0;
}

int flash_trace_record(FlashTrace *trace, FlashTraceEventType type,
                       uint32_t address, uint32_t size,
                       const char *region_name) {
    if (!trace || !trace->enabled) return -1;
    if (trace->count >= FLASH_TRACE_MAX_EVENTS) return -1;

    FlashTraceEvent *ev = &trace->events[trace->count];
    ev->type    = type;
    ev->address = address;
    ev->size    = size;
    ev->timestamp = time(NULL);
    ev->sequence  = trace->next_seq++;

    if (region_name)
        strncpy(ev->region_name, region_name, FLASH_TRACE_NAME_LEN - 1);
    else
        ev->region_name[0] = '\0';

    trace->count++;
    return ev->sequence;
}

int flash_trace_count(const FlashTrace *trace) {
    return trace ? trace->count : 0;
}

const FlashTraceEvent *flash_trace_get(const FlashTrace *trace, int index) {
    if (!trace || index < 0 || index >= trace->count) return NULL;
    return &trace->events[index];
}

int flash_trace_find_by_region(const FlashTrace *trace, const char *region_name,
                               FlashTraceEvent *out, int out_max) {
    if (!trace || !region_name || !out || out_max <= 0) return 0;
    int found = 0;
    for (int i = 0; i < trace->count && found < out_max; i++) {
        if (strcmp(trace->events[i].region_name, region_name) == 0)
            out[found++] = trace->events[i];
    }
    return found;
}

int flash_trace_find_by_type(const FlashTrace *trace, FlashTraceEventType type,
                             FlashTraceEvent *out, int out_max) {
    if (!trace || !out || out_max <= 0) return 0;
    int found = 0;
    for (int i = 0; i < trace->count && found < out_max; i++) {
        if (trace->events[i].type == type)
            out[found++] = trace->events[i];
    }
    return found;
}

void flash_trace_print(const FlashTrace *trace) {
    if (!trace) return;
    printf("Flash Trace Log (%d events):\n", trace->count);
    printf("  %-4s  %-8s  %-10s  %-10s  %s\n",
           "Seq", "Type", "Address", "Size", "Region");
    printf("  %s\n", "----------------------------------------------------");
    for (int i = 0; i < trace->count; i++) {
        const FlashTraceEvent *ev = &trace->events[i];
        printf("  %-4d  %-8s  0x%08X  0x%08X  %s\n",
               ev->sequence, event_type_str(ev->type),
               ev->address, ev->size, ev->region_name);
    }
}

void flash_trace_print_summary(const FlashTrace *trace) {
    if (!trace) return;
    int counts[4] = {0};
    for (int i = 0; i < trace->count; i++)
        if (trace->events[i].type <= TRACE_EVENT_VERIFY)
            counts[trace->events[i].type]++;
    printf("Trace Summary: total=%d READ=%d WRITE=%d ERASE=%d VERIFY=%d\n",
           trace->count, counts[0], counts[1], counts[2], counts[3]);
}
