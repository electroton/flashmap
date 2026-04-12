#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/flashnotify.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t size, uint32_t used) {
    FlashRegion r;
    memset(&r, 0, sizeof(r));
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.size = size;
    r.used = used;
    return r;
}

static void test_init(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    assert(set.rule_count == 0);
    printf("PASS: test_init\n");
}

static void test_add_rule(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    int rc = flashnotify_add_rule(&set, "FLASH", 80, NOTIFY_LEVEL_WARNING);
    assert(rc == 0);
    assert(set.rule_count == 1);
    assert(set.rules[0].threshold_pct == 80);
    assert(set.rules[0].level == NOTIFY_LEVEL_WARNING);
    printf("PASS: test_add_rule\n");
}

static void test_add_rule_clamps_threshold(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    flashnotify_add_rule(&set, "ROM", 150, NOTIFY_LEVEL_INFO);
    assert(set.rules[0].threshold_pct == 100);
    printf("PASS: test_add_rule_clamps_threshold\n");
}

static void test_evaluate_triggered(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    flashnotify_add_rule(&set, "FLASH", 75, NOTIFY_LEVEL_WARNING);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 1024, 800); /* 78% used */

    flashnotify_evaluate(&set, regions, 1);
    assert(set.rules[0].triggered == true);
    printf("PASS: test_evaluate_triggered\n");
}

static void test_evaluate_not_triggered(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    flashnotify_add_rule(&set, "FLASH", 90, NOTIFY_LEVEL_CRITICAL);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 1024, 512); /* 50% used */

    flashnotify_evaluate(&set, regions, 1);
    assert(set.rules[0].triggered == false);
    printf("PASS: test_evaluate_not_triggered\n");
}

static void test_evaluate_unknown_region(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    flashnotify_add_rule(&set, "EEPROM", 50, NOTIFY_LEVEL_INFO);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 1024, 600);

    flashnotify_evaluate(&set, regions, 1);
    assert(set.rules[0].triggered == false);
    printf("PASS: test_evaluate_unknown_region\n");
}

static void test_max_level(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    flashnotify_add_rule(&set, "FLASH", 50, NOTIFY_LEVEL_WARNING);
    flashnotify_add_rule(&set, "RAM",   80, NOTIFY_LEVEL_CRITICAL);

    FlashRegion regions[2];
    regions[0] = make_region("FLASH", 1024, 600); /* 58% -> triggered */
    regions[1] = make_region("RAM",   512,  100); /* 19% -> not triggered */

    flashnotify_evaluate(&set, regions, 2);
    FlashNotifyLevel max = flashnotify_max_level(&set);
    assert(max == NOTIFY_LEVEL_WARNING);
    printf("PASS: test_max_level\n");
}

static void test_max_level_none_triggered(void) {
    FlashNotifySet set;
    flashnotify_init(&set);
    flashnotify_add_rule(&set, "FLASH", 95, NOTIFY_LEVEL_CRITICAL);

    FlashRegion regions[1];
    regions[0] = make_region("FLASH", 1024, 10);

    flashnotify_evaluate(&set, regions, 1);
    assert(flashnotify_max_level(&set) == NOTIFY_LEVEL_INFO);
    printf("PASS: test_max_level_none_triggered\n");
}

int main(void) {
    printf("=== flashnotify tests ===\n");
    test_init();
    test_add_rule();
    test_add_rule_clamps_threshold();
    test_evaluate_triggered();
    test_evaluate_not_triggered();
    test_evaluate_unknown_region();
    test_max_level();
    test_max_level_none_triggered();
    printf("All flashnotify tests passed.\n");
    return 0;
}
