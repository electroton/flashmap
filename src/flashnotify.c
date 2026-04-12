#include "flashnotify.h"
#include <stdio.h>
#include <string.h>

static const char *level_str(FlashNotifyLevel level) {
    switch (level) {
        case NOTIFY_LEVEL_INFO:     return "INFO";
        case NOTIFY_LEVEL_WARNING:  return "WARNING";
        case NOTIFY_LEVEL_CRITICAL: return "CRITICAL";
        default:                    return "UNKNOWN";
    }
}

void flashnotify_init(FlashNotifySet *set) {
    if (!set) return;
    memset(set, 0, sizeof(*set));
}

int flashnotify_add_rule(FlashNotifySet *set,
                         const char *region_name,
                         uint8_t threshold_pct,
                         FlashNotifyLevel level) {
    if (!set || !region_name) return -1;
    if (set->rule_count >= FLASHNOTIFY_MAX_RULES) return -1;
    if (threshold_pct > 100) threshold_pct = 100;

    FlashNotifyRule *rule = &set->rules[set->rule_count++];
    strncpy(rule->region_name, region_name, FLASHNOTIFY_NAME_LEN - 1);
    rule->region_name[FLASHNOTIFY_NAME_LEN - 1] = '\0';
    rule->threshold_pct = threshold_pct;
    rule->level         = level;
    rule->triggered     = false;
    return 0;
}

void flashnotify_evaluate(FlashNotifySet *set,
                          const FlashRegion *regions,
                          int region_count) {
    if (!set || !regions || region_count <= 0) return;

    for (int i = 0; i < set->rule_count; i++) {
        FlashNotifyRule *rule = &set->rules[i];
        rule->triggered = false;

        for (int j = 0; j < region_count; j++) {
            const FlashRegion *r = &regions[j];
            if (strncmp(r->name, rule->region_name, FLASHNOTIFY_NAME_LEN) != 0)
                continue;
            if (r->size == 0) break;

            uint8_t used_pct = (uint8_t)((r->used * 100UL) / r->size);
            if (used_pct >= rule->threshold_pct)
                rule->triggered = true;
            break;
        }
    }
}

void flashnotify_report(const FlashNotifySet *set) {
    if (!set) return;
    bool any = false;
    for (int i = 0; i < set->rule_count; i++) {
        const FlashNotifyRule *rule = &set->rules[i];
        if (!rule->triggered) continue;
        printf("[%s] Region '%s' has reached %u%% usage threshold\n",
               level_str(rule->level),
               rule->region_name,
               rule->threshold_pct);
        any = true;
    }
    if (!any)
        printf("[INFO] No threshold notifications triggered.\n");
}

FlashNotifyLevel flashnotify_max_level(const FlashNotifySet *set) {
    FlashNotifyLevel max = NOTIFY_LEVEL_INFO;
    if (!set) return max;
    for (int i = 0; i < set->rule_count; i++) {
        if (set->rules[i].triggered && set->rules[i].level > max)
            max = set->rules[i].level;
    }
    return max;
}
