#include "flashprofile.h"
#include "flashsummary.h"
#include <stdio.h>
#include <string.h>

void flash_profile_init(FlashProfile *profile, const char *name) {
    memset(profile, 0, sizeof(*profile));
    strncpy(profile->name, name, FLASH_PROFILE_NAME_MAX - 1);
}

bool flash_profile_add_rule(FlashProfile *profile, FlashProfileRule rule) {
    if (profile->rule_count >= FLASH_PROFILE_MAX_RULES) {
        return false;
    }
    profile->rules[profile->rule_count++] = rule;
    return true;
}

int flash_profile_evaluate(const FlashProfile *profile,
                            const FlashLayout  *layout,
                            FlashProfileResult *results,
                            int                 results_capacity) {
    if (!profile || !layout || !results) return 0;

    FlashSummary summary;
    flash_summary_compute(layout, &summary);

    int count = 0;
    for (int i = 0; i < profile->rule_count && count < results_capacity; i++) {
        const FlashProfileRule *rule = &profile->rules[i];
        FlashProfileResult     *res  = &results[count++];
        res->rule_index = i;
        res->passed     = false;

        switch (rule->type) {
            case PROFILE_RULE_MIN_FREE:
                res->passed = (summary.free_bytes >= rule->value);
                snprintf(res->message, sizeof(res->message),
                         "MIN_FREE: required=%u, actual=%u",
                         rule->value, summary.free_bytes);
                break;

            case PROFILE_RULE_MAX_USED:
                res->passed = (summary.used_bytes <= rule->value);
                snprintf(res->message, sizeof(res->message),
                         "MAX_USED: limit=%u, actual=%u",
                         rule->value, summary.used_bytes);
                break;

            case PROFILE_RULE_REGION_PRESENT: {
                bool found = false;
                for (int r = 0; r < layout->region_count; r++) {
                    if (strcmp(layout->regions[r].name, rule->region_name) == 0) {
                        found = true;
                        break;
                    }
                }
                res->passed = found;
                snprintf(res->message, sizeof(res->message),
                         "REGION_PRESENT: '%s' %s", rule->region_name,
                         found ? "found" : "missing");
                break;
            }

            case PROFILE_RULE_REGION_ABSENT: {
                bool found = false;
                for (int r = 0; r < layout->region_count; r++) {
                    if (strcmp(layout->regions[r].name, rule->region_name) == 0) {
                        found = true;
                        break;
                    }
                }
                res->passed = !found;
                snprintf(res->message, sizeof(res->message),
                         "REGION_ABSENT: '%s' %s", rule->region_name,
                         found ? "unexpectedly present" : "correctly absent");
                break;
            }

            case PROFILE_RULE_ALIGN_CHECK: {
                bool all_aligned = true;
                for (int r = 0; r < layout->region_count; r++) {
                    uint32_t addr = layout->regions[r].start_address;
                    if (rule->value > 0 && (addr % rule->value) != 0) {
                        all_aligned = false;
                        break;
                    }
                }
                res->passed = all_aligned;
                snprintf(res->message, sizeof(res->message),
                         "ALIGN_CHECK: alignment=%u — %s",
                         rule->value, all_aligned ? "OK" : "FAIL");
                break;
            }
        }
    }
    return count;
}

void flash_profile_print_report(const FlashProfile       *profile,
                                 const FlashProfileResult *results,
                                 int                       result_count) {
    printf("=== Flash Profile: %s ===\n", profile->name);
    int passed = 0;
    for (int i = 0; i < result_count; i++) {
        printf("  [%s] Rule %d: %s\n",
               results[i].passed ? "PASS" : "FAIL",
               results[i].rule_index,
               results[i].message);
        if (results[i].passed) passed++;
    }
    printf("Result: %d/%d rules passed\n", passed, result_count);
}
