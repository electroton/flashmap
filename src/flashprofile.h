#ifndef FLASHPROFILE_H
#define FLASHPROFILE_H

#include "flashlayout.h"
#include "flashregion.h"
#include <stdint.h>
#include <stdbool.h>

/* Flash profile: named configuration describing expected memory layout constraints */

#define FLASH_PROFILE_NAME_MAX  64
#define FLASH_PROFILE_MAX_RULES 16

typedef enum {
    PROFILE_RULE_MIN_FREE,    /* minimum free bytes required */
    PROFILE_RULE_MAX_USED,    /* maximum used bytes allowed */
    PROFILE_RULE_REGION_PRESENT, /* a named region must exist */
    PROFILE_RULE_REGION_ABSENT,  /* a named region must not exist */
    PROFILE_RULE_ALIGN_CHECK  /* all regions must be aligned to given power-of-2 */
} FlashProfileRuleType;

typedef struct {
    FlashProfileRuleType type;
    char   region_name[FLASH_PROFILE_NAME_MAX]; /* used by PRESENT/ABSENT rules */
    uint32_t value;                              /* used by MIN_FREE/MAX_USED/ALIGN_CHECK */
} FlashProfileRule;

typedef struct {
    char             name[FLASH_PROFILE_NAME_MAX];
    FlashProfileRule rules[FLASH_PROFILE_MAX_RULES];
    int              rule_count;
} FlashProfile;

typedef struct {
    bool     passed;
    int      rule_index;
    char     message[128];
} FlashProfileResult;

/* Initialise a profile with a given name */
void flash_profile_init(FlashProfile *profile, const char *name);

/* Add a rule to the profile; returns false if rule limit reached */
bool flash_profile_add_rule(FlashProfile *profile, FlashProfileRule rule);

/* Evaluate all rules against a layout; results array must hold at least rule_count entries */
int  flash_profile_evaluate(const FlashProfile *profile,
                             const FlashLayout  *layout,
                             FlashProfileResult  *results,
                             int                  results_capacity);

/* Print a human-readable profile report to stdout */
void flash_profile_print_report(const FlashProfile       *profile,
                                 const FlashProfileResult *results,
                                 int                       result_count);

#endif /* FLASHPROFILE_H */
