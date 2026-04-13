#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashtrace.h"

static void test_init(void) {
    FlashTrace trace;
    flashtrace_init(&trace);
    assert(trace.count == 0);
    printf("[PASS] test_init\n");
}

static void test_record_single(void) {
    FlashTrace trace;
    flashtrace_init(&trace);

    int ret = flashtrace_record(&trace, "FLASH_TEXT", 0x08000000, 0x4000,
                                TRACE_ACCESS_READ);
    assert(ret == 0);
    assert(trace.count == 1);
    assert(strcmp(trace.entries[0].region_name, "FLASH_TEXT") == 0);
    assert(trace.entries[0].address == 0x08000000);
    assert(trace.entries[0].size == 0x4000);
    assert(trace.entries[0].access_type == TRACE_ACCESS_READ);
    assert(trace.entries[0].hit_count == 1);
    printf("[PASS] test_record_single\n");
}

static void test_record_increments_hit_count(void) {
    FlashTrace trace;
    flashtrace_init(&trace);

    flashtrace_record(&trace, "FLASH_DATA", 0x08004000, 0x1000,
                      TRACE_ACCESS_WRITE);
    flashtrace_record(&trace, "FLASH_DATA", 0x08004000, 0x1000,
                      TRACE_ACCESS_WRITE);
    flashtrace_record(&trace, "FLASH_DATA", 0x08004000, 0x1000,
                      TRACE_ACCESS_WRITE);

    assert(trace.count == 1);
    assert(trace.entries[0].hit_count == 3);
    printf("[PASS] test_record_increments_hit_count\n");
}

static void test_record_different_access_types(void) {
    FlashTrace trace;
    flashtrace_init(&trace);

    flashtrace_record(&trace, "BOOT", 0x08000000, 0x200, TRACE_ACCESS_READ);
    flashtrace_record(&trace, "BOOT", 0x08000000, 0x200, TRACE_ACCESS_EXEC);

    assert(trace.count == 2);
    printf("[PASS] test_record_different_access_types\n");
}

static void test_find_existing(void) {
    FlashTrace trace;
    flashtrace_init(&trace);

    flashtrace_record(&trace, "CONFIG", 0x0803F000, 0x800, TRACE_ACCESS_READ);

    const FlashTraceEntry *e = flashtrace_find(&trace, "CONFIG",
                                               TRACE_ACCESS_READ);
    assert(e != NULL);
    assert(e->address == 0x0803F000);
    assert(e->hit_count == 1);
    printf("[PASS] test_find_existing\n");
}

static void test_find_missing(void) {
    FlashTrace trace;
    flashtrace_init(&trace);

    const FlashTraceEntry *e = flashtrace_find(&trace, "NONEXISTENT",
                                               TRACE_ACCESS_READ);
    assert(e == NULL);
    printf("[PASS] test_find_missing\n");
}

static void test_reset(void) {
    FlashTrace trace;
    flashtrace_init(&trace);

    flashtrace_record(&trace, "A", 0x1000, 0x100, TRACE_ACCESS_READ);
    flashtrace_record(&trace, "B", 0x2000, 0x200, TRACE_ACCESS_WRITE);
    assert(trace.count == 2);

    flashtrace_reset(&trace);
    assert(trace.count == 0);
    printf("[PASS] test_reset\n");
}

int main(void) {
    printf("=== flashtrace tests ===\n");
    test_init();
    test_record_single();
    test_record_increments_hit_count();
    test_record_different_access_types();
    test_find_existing();
    test_find_missing();
    test_reset();
    printf("All flashtrace tests passed.\n");
    return 0;
}
