/*
 * flashtrace.h - Flash memory region access tracing
 */
#ifndef FLASHTRACE_H
#define FLASHTRACE_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include "flashregion.h"

#define FLASH_TRACE_MAX_EVENTS 256
#define FLASH_TRACE_NAME_LEN   64

typedef enum {
    TRACE_EVENT_READ,
    TRACE_EVENT_WRITE,
    TRACE_EVENT_ERASE,
    TRACE_EVENT_VERIFY
} FlashTraceEventType;

typedef struct {
    FlashTraceEventType type;
    uint32_t            address;
    uint32_t            size;
    char                region_name[FLASH_TRACE_NAME_LEN];
    time_t              timestamp;
    int                 sequence;
} FlashTraceEvent;

typedef struct {
    FlashTraceEvent events[FLASH_TRACE_MAX_EVENTS];
    int             count;
    int             next_seq;
    int             enabled;
} FlashTrace;

/* Lifecycle */
void flash_trace_init(FlashTrace *trace);
void flash_trace_reset(FlashTrace *trace);
void flash_trace_enable(FlashTrace *trace);
void flash_trace_disable(FlashTrace *trace);

/* Recording */
int  flash_trace_record(FlashTrace *trace, FlashTraceEventType type,
                        uint32_t address, uint32_t size,
                        const char *region_name);

/* Querying */
int  flash_trace_count(const FlashTrace *trace);
const FlashTraceEvent *flash_trace_get(const FlashTrace *trace, int index);
int  flash_trace_find_by_region(const FlashTrace *trace, const char *region_name,
                                FlashTraceEvent *out, int out_max);
int  flash_trace_find_by_type(const FlashTrace *trace, FlashTraceEventType type,
                              FlashTraceEvent *out, int out_max);

/* Reporting */
void flash_trace_print(const FlashTrace *trace);
void flash_trace_print_summary(const FlashTrace *trace);

#endif /* FLASHTRACE_H */
