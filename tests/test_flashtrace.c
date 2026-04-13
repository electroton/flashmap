/*
 * test_flashtrace.c - Unit tests for flashtrace module
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashtrace.h"
#include "../src/flashtrace_report.h"

static void test_init_and_reset(void) {
    FlashTrace trace;
    flash_trace_init(&trace);
    assert(trace.count    == 0);
    assert(trace.next_seq == 0);
    assert(trace.enabled  == 1);

    flash_trace_record(&trace, TRACE_EVENT_READ, 0x1000, 256, "bootloader");
    assert(trace.count == 1);

    flash_trace_reset(&trace);
    assert(trace.count    == 0);
    assert(trace.next_seq == 0);
    printf("PASS: test_init_and_reset\n");
}

static void test_record_and_get(void) {
    FlashTrace trace;
    flash_trace_init(&trace);

    int seq = flash_trace_record(&trace, TRACE_EVENT_WRITE, 0x2000, 512, "firmware");
    assert(seq == 0);
    assert(flash_trace_count(&trace) == 1);

    const FlashTraceEvent *ev = flash_trace_get(&trace, 0);
    assert(ev != NULL);
    assert(ev->type    == TRACE_EVENT_WRITE);
    assert(ev->address == 0x2000);
    assert(ev->size    == 512);
    assert(strcmp(ev->region_name, "firmware") == 0);
    assert(ev->sequence == 0);

    assert(flash_trace_get(&trace, 1)  == NULL);
    assert(flash_trace_get(&trace, -1) == NULL);
    printf("PASS: test_record_and_get\n");
}

static void test_enable_disable(void) {
    FlashTrace trace;
    flash_trace_init(&trace);

    flash_trace_disable(&trace);
    int seq = flash_trace_record(&trace, TRACE_EVENT_ERASE, 0x3000, 4096, "data");
    assert(seq == -1);
    assert(flash_trace_count(&trace) == 0);

    flash_trace_enable(&trace);
    seq = flash_trace_record(&trace, TRACE_EVENT_ERASE, 0x3000, 4096, "data");
    assert(seq == 0);
    assert(flash_trace_count(&trace) == 1);
    printf("PASS: test_enable_disable\n");
}

static void test_find_by_region(void) {
    FlashTrace trace;
    flash_trace_init(&trace);

    flash_trace_record(&trace, TRACE_EVENT_READ,  0x1000, 128, "boot");
    flash_trace_record(&trace, TRACE_EVENT_WRITE, 0x2000, 256, "app");
    flash_trace_record(&trace, TRACE_EVENT_READ,  0x1100, 64,  "boot");
    flash_trace_record(&trace, TRACE_EVENT_ERASE, 0x3000, 512, "data");

    FlashTraceEvent buf[16];
    int n = flash_trace_find_by_region(&trace, "boot", buf, 16);
    assert(n == 2);
    assert(buf[0].address == 0x1000);
    assert(buf[1].address == 0x1100);

    n = flash_trace_find_by_region(&trace, "missing", buf, 16);
    assert(n == 0);
    printf("PASS: test_find_by_region\n");
}

static void test_find_by_type(void) {
    FlashTrace trace;
    flash_trace_init(&trace);

    flash_trace_record(&trace, TRACE_EVENT_READ,   0x1000, 64,  "r1");
    flash_trace_record(&trace, TRACE_EVENT_WRITE,  0x2000, 128, "r2");
    flash_trace_record(&trace, TRACE_EVENT_READ,   0x1080, 64,  "r1");
    flash_trace_record(&trace, TRACE_EVENT_VERIFY, 0x2000, 128, "r2");

    FlashTraceEvent buf[16];
    int n = flash_trace_find_by_type(&trace, TRACE_EVENT_READ, buf, 16);
    assert(n == 2);

    n = flash_trace_find_by_type(&trace, TRACE_EVENT_ERASE, buf, 16);
    assert(n == 0);
    printf("PASS: test_find_by_type\n");
}

static void test_report_csv(void) {
    FlashTrace trace;
    flash_trace_init(&trace);

    flash_trace_record(&trace, TRACE_EVENT_READ,  0xA000, 256, "sector0");
    flash_trace_record(&trace, TRACE_EVENT_WRITE, 0xB000, 512, "sector1");

    int rc = flash_trace_report_csv(&trace, "/tmp/flashtrace_test.csv");
    assert(rc == 0);

    rc = flash_trace_report_csv(NULL, "/tmp/flashtrace_test.csv");
    assert(rc == -1);

    rc = flash_trace_report_csv(&trace, NULL);
    assert(rc == -1);
    printf("PASS: test_report_csv\n");
}

int main(void) {
    printf("Running flashtrace tests...\n");
    test_init_and_reset();
    test_record_and_get();
    test_enable_disable();
    test_find_by_region();
    test_find_by_type();
    test_report_csv();
    printf("All flashtrace tests passed.\n");
    return 0;
}
