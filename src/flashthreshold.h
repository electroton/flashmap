#ifndef FLASHTHRESHOLD_H
#define FLASHTHRESHOLD_H

#include <stdint.h>
#include <stdbool.h>
#include "flashregion.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Threshold level classification */
typedef enum {
    THRESHOLD_OK      = 0,
    THRESHOLD_WARNING = 1,
    THRESHOLD_CRITICAL = 2
} ThresholdLevel;

/* Threshold configuration for a flash region */
typedef struct {
    char     region_name[64];
    uint32_t warning_percent;   /* 0-100 */
    uint32_t critical_percent;  /* 0-100 */
} FlashThresholdConfig;

/* Result of a threshold evaluation */
typedef struct {
    char           region_name[64];
    uint32_t       used_bytes;
    uint32_t       total_bytes;
    uint32_t       used_percent;
    ThresholdLevel level;
} FlashThresholdResult;

/* Context holding multiple threshold configs */
typedef struct {
    FlashThresholdConfig *configs;
    int                   count;
    int                   capacity;
} FlashThresholdContext;

FlashThresholdContext *flash_threshold_context_create(void);
void                   flash_threshold_context_free(FlashThresholdContext *ctx);

int  flash_threshold_add(FlashThresholdContext *ctx,
                         const char *region_name,
                         uint32_t warning_percent,
                         uint32_t critical_percent);

ThresholdLevel flash_threshold_evaluate(const FlashThresholdContext *ctx,
                                        const FlashRegion *region,
                                        FlashThresholdResult *out_result);

int flash_threshold_evaluate_all(const FlashThresholdContext *ctx,
                                 const FlashRegion *regions,
                                 int region_count,
                                 FlashThresholdResult *out_results,
                                 int max_results);

const char *flash_threshold_level_str(ThresholdLevel level);

#ifdef __cplusplus
}
#endif

#endif /* FLASHTHRESHOLD_H */
