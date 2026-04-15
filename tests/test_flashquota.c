#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashquota.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.size = size;
    r.used = used;
    return r;
}

static FlashQuotaRule make_rule(const char *name, uint32_t max_used,
                                float max_pct, FlashQuotaMode mode) {
    FlashQuotaRule rule;
    memset(&rule, 0, sizeof(rule));
    strncpy(rule.region_name, name, sizeof(rule.region_name) - 1);
    rule.max_used    = max_used;
    rule.max_percent = max_pct;
    rule.mode        = mode;
    return rule;
}

static void test_no_violation(void) {
    FlashRegion regions[] = {
        make_region("FLASH", 0x40000, 0x10000)
    };
    FlashQuotaRule rules[] = {
        make_rule("FLASH", 0x20000, 80.0f, QUOTA_MODE_WARN)
    };

    FlashQuotaReport report = flash_quota_check(rules, 1, regions, 1);
    assert(report.count == 1);
    assert(report.results[0].violated == 0);
    assert(report.any_error == 0);
    flash_quota_report_free(&report);
    printf("PASS: test_no_violation\n");
}

static void test_byte_violation_warn(void) {
    FlashRegion regions[] = {
        make_region("FLASH", 0x40000, 0x30000)
    };
    FlashQuotaRule rules[] = {
        make_rule("FLASH", 0x20000, 100.0f, QUOTA_MODE_WARN)
    };

    FlashQuotaReport report = flash_quota_check(rules, 1, regions, 1);
    assert(report.count == 1);
    assert(report.results[0].violated == 1);
    assert(report.any_error == 0); /* WARN mode — not an error */
    flash_quota_report_free(&report);
    printf("PASS: test_byte_violation_warn\n");
}

static void test_percent_violation_error(void) {
    FlashRegion regions[] = {
        make_region("RAM", 0x8000, 0x7800) /* 93.75% used */
    };
    FlashQuotaRule rules[] = {
        make_rule("RAM", UINT32_MAX, 90.0f, QUOTA_MODE_ERROR)
    };

    FlashQuotaReport report = flash_quota_check(rules, 1, regions, 1);
    assert(report.count == 1);
    assert(report.results[0].violated == 1);
    assert(report.any_error == 1);
    flash_quota_report_free(&report);
    printf("PASS: test_percent_violation_error\n");
}

static void test_unknown_region(void) {
    FlashRegion regions[] = {
        make_region("FLASH", 0x40000, 0x1000)
    };
    FlashQuotaRule rules[] = {
        make_rule("NVRAM", 0x1000, 50.0f, QUOTA_MODE_ERROR)
    };

    FlashQuotaReport report = flash_quota_check(rules, 1, regions, 1);
    assert(report.count == 1);
    /* Unknown region: not violated, just unmatched */
    assert(report.results[0].violated == 0);
    flash_quota_report_free(&report);
    printf("PASS: test_unknown_region\n");
}

int main(void) {
    test_no_violation();
    test_byte_violation_warn();
    test_percent_violation_error();
    test_unknown_region();
    printf("All flashquota tests passed.\n");
    return 0;
}
