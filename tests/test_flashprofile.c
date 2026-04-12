#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashprofile.h"
#include "../src/flashprofile_report.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_test_layout(void) {
    FlashLayout layout;
    flash_layout_init(&layout);

    FlashRegion r1 = { .name = "bootloader", .start_address = 0x08000000,
                       .size = 0x4000,  .used = 0x3800 };
    FlashRegion r2 = { .name = "application", .start_address = 0x08004000,
                       .size = 0x3C000, .used = 0x20000 };
    FlashRegion r3 = { .name = "config", .start_address = 0x08040000,
                       .size = 0x1000,  .used = 0x200 };

    flash_layout_add_region(&layout, r1);
    flash_layout_add_region(&layout, r2);
    flash_layout_add_region(&layout, r3);
    return layout;
}

static void test_profile_init(void) {
    FlashProfile p;
    flash_profile_init(&p, "release");
    assert(strcmp(p.name, "release") == 0);
    assert(p.rule_count == 0);
    printf("PASS: test_profile_init\n");
}

static void test_add_rule_limit(void) {
    FlashProfile p;
    flash_profile_init(&p, "test");
    FlashProfileRule rule = { .type = PROFILE_RULE_MIN_FREE, .value = 1024 };
    for (int i = 0; i < FLASH_PROFILE_MAX_RULES; i++) {
        assert(flash_profile_add_rule(&p, rule) == true);
    }
    assert(flash_profile_add_rule(&p, rule) == false);
    printf("PASS: test_add_rule_limit\n");
}

static void test_rule_min_free_pass(void) {
    FlashLayout layout = make_test_layout();
    FlashProfile p;
    flash_profile_init(&p, "min_free");
    FlashProfileRule rule = { .type = PROFILE_RULE_MIN_FREE, .value = 100 };
    flash_profile_add_rule(&p, rule);

    FlashProfileResult results[8];
    int count = flash_profile_evaluate(&p, &layout, results, 8);
    assert(count == 1);
    assert(results[0].passed == true);
    printf("PASS: test_rule_min_free_pass\n");
}

static void test_rule_region_present_fail(void) {
    FlashLayout layout = make_test_layout();
    FlashProfile p;
    flash_profile_init(&p, "region_check");
    FlashProfileRule rule = { .type = PROFILE_RULE_REGION_PRESENT };
    strncpy(rule.region_name, "nonexistent", FLASH_PROFILE_NAME_MAX - 1);
    flash_profile_add_rule(&p, rule);

    FlashProfileResult results[8];
    int count = flash_profile_evaluate(&p, &layout, results, 8);
    assert(count == 1);
    assert(results[0].passed == false);
    printf("PASS: test_rule_region_present_fail\n");
}

static void test_rule_region_absent_pass(void) {
    FlashLayout layout = make_test_layout();
    FlashProfile p;
    flash_profile_init(&p, "absent_check");
    FlashProfileRule rule = { .type = PROFILE_RULE_REGION_ABSENT };
    strncpy(rule.region_name, "debug", FLASH_PROFILE_NAME_MAX - 1);
    flash_profile_add_rule(&p, rule);

    FlashProfileResult results[8];
    int count = flash_profile_evaluate(&p, &layout, results, 8);
    assert(count == 1);
    assert(results[0].passed == true);
    printf("PASS: test_rule_region_absent_pass\n");
}

static void test_rule_align_check(void) {
    FlashLayout layout = make_test_layout();
    FlashProfile p;
    flash_profile_init(&p, "align");
    FlashProfileRule rule = { .type = PROFILE_RULE_ALIGN_CHECK, .value = 0x1000 };
    flash_profile_add_rule(&p, rule);

    FlashProfileResult results[8];
    int count = flash_profile_evaluate(&p, &layout, results, 8);
    assert(count == 1);
    assert(results[0].passed == true);
    printf("PASS: test_rule_align_check\n");
}

static void test_report_to_buffer(void) {
    FlashLayout layout = make_test_layout();
    FlashProfile p;
    flash_profile_init(&p, "buf_test");
    FlashProfileRule rule = { .type = PROFILE_RULE_MAX_USED, .value = 0xFFFFFF };
    flash_profile_add_rule(&p, rule);

    FlashProfileResult results[8];
    int count = flash_profile_evaluate(&p, &layout, results, 8);

    char buf[2048];
    int  len = flash_profile_report_to_buffer(&p, results, count, buf, sizeof(buf));
    assert(len > 0);
    assert(strstr(buf, "buf_test") != NULL);
    assert(strstr(buf, "PASS") != NULL);
    printf("PASS: test_report_to_buffer\n");
}

int main(void) {
    test_profile_init();
    test_add_rule_limit();
    test_rule_min_free_pass();
    test_rule_region_present_fail();
    test_rule_region_absent_pass();
    test_rule_align_check();
    test_report_to_buffer();
    printf("All flashprofile tests passed.\n");
    return 0;
}
