#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/flashaudit.h"

static FlashLayout make_layout(void) {
    FlashLayout layout;
    memset(&layout, 0, sizeof(layout));
    layout.region_count = 3;
    layout.regions = calloc(3, sizeof(FlashRegion));

    strncpy(layout.regions[0].name, "FLASH", sizeof(layout.regions[0].name)-1);
    layout.regions[0].start = 0x08000000;
    layout.regions[0].size  = 0x80000;  /* 512 KB */
    layout.regions[0].used  = 0x70000;  /* 87.5% used */

    strncpy(layout.regions[1].name, "RAM", sizeof(layout.regions[1].name)-1);
    layout.regions[1].start = 0x20000000;
    layout.regions[1].size  = 0x20000;  /* 128 KB */
    layout.regions[1].used  = 0x5000;

    strncpy(layout.regions[2].name, "EEPROM", sizeof(layout.regions[2].name)-1);
    layout.regions[2].start = 0x08080003; /* intentionally misaligned */
    layout.regions[2].size  = 0x800;
    layout.regions[2].used  = 0x100;

    return layout;
}

static void free_layout(FlashLayout *l) { free(l->regions); }

void test_no_findings_when_rules_pass(void) {
    FlashLayout layout = make_layout();
    AuditRule rules[1] = {{
        .type = AUDIT_RULE_MAX_SIZE,
        .region_name = "",
        .threshold = 0x100000,
        .severity = AUDIT_ERROR
    }};
    AuditResult *r = flash_audit_run(&layout, rules, 1);
    assert(r != NULL);
    assert(r->count == 0);
    flash_audit_result_free(r);
    free_layout(&layout);
    printf("PASS: test_no_findings_when_rules_pass\n");
}

void test_fill_threshold_triggers(void) {
    FlashLayout layout = make_layout();
    AuditRule rules[1] = {{
        .type = AUDIT_RULE_FILL_THRESHOLD,
        .region_name = "FLASH",
        .threshold = 80, /* 80% */
        .severity = AUDIT_WARNING
    }};
    AuditResult *r = flash_audit_run(&layout, rules, 1);
    assert(r != NULL);
    assert(r->count == 1);
    assert(r->findings[0].severity == AUDIT_WARNING);
    assert(strstr(r->findings[0].message, "FLASH") != NULL);
    flash_audit_result_free(r);
    free_layout(&layout);
    printf("PASS: test_fill_threshold_triggers\n");
}

void test_alignment_check(void) {
    FlashLayout layout = make_layout();
    AuditRule rules[1] = {{
        .type = AUDIT_RULE_ALIGNMENT,
        .region_name = "EEPROM",
        .threshold = 4,
        .severity = AUDIT_ERROR
    }};
    AuditResult *r = flash_audit_run(&layout, rules, 1);
    assert(r != NULL);
    assert(r->count == 1);
    assert(flash_audit_has_errors(r));
    flash_audit_result_free(r);
    free_layout(&layout);
    printf("PASS: test_alignment_check\n");
}

void test_count_by_severity(void) {
    FlashLayout layout = make_layout();
    AuditRule rules[2] = {
        { AUDIT_RULE_FILL_THRESHOLD, "FLASH",  80, AUDIT_WARNING },
        { AUDIT_RULE_ALIGNMENT,     "EEPROM",   4, AUDIT_ERROR   }
    };
    AuditResult *r = flash_audit_run(&layout, rules, 2);
    assert(flash_audit_count_by_severity(r, AUDIT_WARNING) == 1);
    assert(flash_audit_count_by_severity(r, AUDIT_ERROR)   == 1);
    assert(flash_audit_count_by_severity(r, AUDIT_INFO)    == 0);
    flash_audit_result_free(r);
    free_layout(&layout);
    printf("PASS: test_count_by_severity\n");
}

int main(void) {
    test_no_findings_when_rules_pass();
    test_fill_threshold_triggers();
    test_alignment_check();
    test_count_by_severity();
    printf("All flashaudit tests passed.\n");
    return 0;
}
