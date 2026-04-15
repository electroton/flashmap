#include "flashthreshold.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 8

FlashThresholdContext *flash_threshold_context_create(void) {
    FlashThresholdContext *ctx = malloc(sizeof(FlashThresholdContext));
    if (!ctx) return NULL;
    ctx->configs  = malloc(sizeof(FlashThresholdConfig) * INITIAL_CAPACITY);
    if (!ctx->configs) { free(ctx); return NULL; }
    ctx->count    = 0;
    ctx->capacity = INITIAL_CAPACITY;
    return ctx;
}

void flash_threshold_context_free(FlashThresholdContext *ctx) {
    if (!ctx) return;
    free(ctx->configs);
    free(ctx);
}

int flash_threshold_add(FlashThresholdContext *ctx,
                        const char *region_name,
                        uint32_t warning_percent,
                        uint32_t critical_percent) {
    if (!ctx || !region_name) return -1;
    if (warning_percent > 100 || critical_percent > 100) return -1;
    if (warning_percent >= critical_percent) return -1;

    if (ctx->count >= ctx->capacity) {
        int new_cap = ctx->capacity * 2;
        FlashThresholdConfig *tmp = realloc(ctx->configs,
                                            sizeof(FlashThresholdConfig) * new_cap);
        if (!tmp) return -1;
        ctx->configs  = tmp;
        ctx->capacity = new_cap;
    }

    FlashThresholdConfig *cfg = &ctx->configs[ctx->count++];
    strncpy(cfg->region_name, region_name, sizeof(cfg->region_name) - 1);
    cfg->region_name[sizeof(cfg->region_name) - 1] = '\0';
    cfg->warning_percent  = warning_percent;
    cfg->critical_percent = critical_percent;
    return 0;
}

ThresholdLevel flash_threshold_evaluate(const FlashThresholdContext *ctx,
                                        const FlashRegion *region,
                                        FlashThresholdResult *out_result) {
    if (!ctx || !region) return THRESHOLD_OK;

    uint32_t used    = region->used_size;
    uint32_t total   = region->size;
    uint32_t percent = (total > 0) ? (uint32_t)((uint64_t)used * 100 / total) : 0;

    ThresholdLevel level = THRESHOLD_OK;

    for (int i = 0; i < ctx->count; i++) {
        const FlashThresholdConfig *cfg = &ctx->configs[i];
        if (strcmp(cfg->region_name, region->name) == 0) {
            if (percent >= cfg->critical_percent)
                level = THRESHOLD_CRITICAL;
            else if (percent >= cfg->warning_percent)
                level = THRESHOLD_WARNING;
            break;
        }
    }

    if (out_result) {
        strncpy(out_result->region_name, region->name,
                sizeof(out_result->region_name) - 1);
        out_result->region_name[sizeof(out_result->region_name) - 1] = '\0';
        out_result->used_bytes   = used;
        out_result->total_bytes  = total;
        out_result->used_percent = percent;
        out_result->level        = level;
    }
    return level;
}

int flash_threshold_evaluate_all(const FlashThresholdContext *ctx,
                                 const FlashRegion *regions,
                                 int region_count,
                                 FlashThresholdResult *out_results,
                                 int max_results) {
    if (!ctx || !regions || !out_results || max_results <= 0) return 0;
    int n = (region_count < max_results) ? region_count : max_results;
    for (int i = 0; i < n; i++)
        flash_threshold_evaluate(ctx, &regions[i], &out_results[i]);
    return n;
}

const char *flash_threshold_level_str(ThresholdLevel level) {
    switch (level) {
        case THRESHOLD_OK:       return "OK";
        case THRESHOLD_WARNING:  return "WARNING";
        case THRESHOLD_CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}
