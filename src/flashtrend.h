/**
 * flashtrend.h - Flash memory usage trend analysis over time
 *
 * Analyzes how flash region usage changes across multiple snapshots,
 * computing growth rates and projecting future usage.
 */

#ifndef FLASHTREND_H
#define FLASHTREND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_TREND_MAX_POINTS 64
#define FLASH_TREND_NAME_LEN   64

typedef struct {
    uint64_t timestamp;   /* Unix timestamp or monotonic counter */
    uint32_t used_bytes;  /* Bytes used at this point in time */
    uint32_t total_bytes; /* Total capacity at this point */
} FlashTrendPoint;

typedef struct {
    char            region_name[FLASH_TREND_NAME_LEN];
    FlashTrendPoint points[FLASH_TREND_MAX_POINTS];
    size_t          count;
} FlashTrendSeries;

typedef struct {
    double   slope;            /* Bytes per timestamp unit */
    double   intercept;        /* Y-intercept of linear fit */
    double   r_squared;        /* Goodness of fit [0.0, 1.0] */
    uint32_t projected_full;   /* Estimated timestamp when region fills */
    int      is_valid;         /* 0 if insufficient data */
} FlashTrendResult;

/**
 * Initialize a trend series for a named region.
 */
void flash_trend_init(FlashTrendSeries *series, const char *region_name);

/**
 * Add a data point to the series. Returns 0 on success, -1 if full.
 */
int flash_trend_add_point(FlashTrendSeries *series, uint64_t timestamp,
                          uint32_t used_bytes, uint32_t total_bytes);

/**
 * Compute linear regression over the series and fill result.
 * Requires at least 2 points; sets is_valid=0 otherwise.
 */
FlashTrendResult flash_trend_analyze(const FlashTrendSeries *series);

/**
 * Return projected used bytes at a given future timestamp.
 * Returns 0 if result is not valid.
 */
uint32_t flash_trend_project(const FlashTrendResult *result,
                             uint64_t future_timestamp);

/**
 * Reset all points in the series.
 */
void flash_trend_reset(FlashTrendSeries *series);

#ifdef __cplusplus
}
#endif

#endif /* FLASHTREND_H */
