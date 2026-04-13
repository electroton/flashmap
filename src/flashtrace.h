/**
 * flashtrace.h - Flash memory region access tracing
 *
 * Tracks read/write access patterns across flash regions
 * for diagnostics and usage analysis.
 */

#ifndef FLASHTRACE_H
#define FLASHTRACE_H

#include <stdint.h>
#include <stddef.h>
#include "flashregion.h"

#define FLASH_TRACE_MAX_ENTRIES 256
#define FLASH_TRACE_NAME_LEN    64

typedef enum {
    TRACE_ACCESS_READ  = 0,
    TRACE_ACCESS_WRITE = 1,
    TRACE_ACCESS_EXEC  = 2
} FlashTraceAccessType;

typedef struct {
    char                 region_name[FLASH_TRACE_NAME_LEN];
    uint32_t             address;
    uint32_t             size;
    FlashTraceAccessType access_type;
    uint32_t             hit_count;
} FlashTraceEntry;

typedef struct {
    FlashTraceEntry entries[FLASH_TRACE_MAX_ENTRIES];
    size_t          count;
} FlashTrace;

/**
 * Initialize a FlashTrace context.
 */
void flashtrace_init(FlashTrace *trace);

/**
 * Record an access event for a named region.
 * If an entry for the region+type already exists, increments hit_count.
 */
int flashtrace_record(FlashTrace *trace,
                      const char *region_name,
                      uint32_t address,
                      uint32_t size,
                      FlashTraceAccessType access_type);

/**
 * Look up a trace entry by region name and access type.
 * Returns pointer to entry or NULL if not found.
 */
const FlashTraceEntry *flashtrace_find(const FlashTrace *trace,
                                       const char *region_name,
                                       FlashTraceAccessType access_type);

/**
 * Print a formatted trace report to stdout.
 */
void flashtrace_print(const FlashTrace *trace);

/**
 * Reset all entries in the trace.
 */
void flashtrace_reset(FlashTrace *trace);

#endif /* FLASHTRACE_H */
