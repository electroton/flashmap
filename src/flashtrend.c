#include "flashtrend.h"

#include <string.h>
#include <math.h>
#include <stdint.h>

void flash_trend_init(FlashTrendSeries *series, const char *region_name)
{
    if (!series) return;
    memset(series, 0, sizeof(*series));
    if (region_name) {
        strncpy(series->region_name, region_name, FLASH_TREND_NAME_LEN - 1);
        series->region_name[FLASH_TREND_NAME_LEN - 1] = '\0';
    }
}

int flash_trend_add_point(FlashTrendSeries *series, uint64_t timestamp,
                          uint32_t used_bytes, uint32_t total_bytes)
{
    if (!series) return -1;
    if (series->count >= FLASH_TREND_MAX_POINTS) return -1;

    FlashTrendPoint *p = &series->points[series->count];
    p->timestamp   = timestamp;
    p->used_bytes  = used_bytes;
    p->total_bytes = total_bytes;
    series->count++;
    return 0;
}

FlashTrendResult flash_trend_analyze(const FlashTrendSeries *series)
{
    FlashTrendResult result;
    memset(&result, 0, sizeof(result));

    if (!series || series->count < 2) {
        result.is_valid = 0;
        return result;
    }

    size_t n = series->count;
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_xx = 0.0;

    for (size_t i = 0; i < n; i++) {
        double x = (double)series->points[i].timestamp;
        double y = (double)series->points[i].used_bytes;
        sum_x  += x;
        sum_y  += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }

    double denom = (double)n * sum_xx - sum_x * sum_x;
    if (fabs(denom) < 1e-12) {
        result.is_valid = 0;
        return result;
    }

    result.slope     = ((double)n * sum_xy - sum_x * sum_y) / denom;
    result.intercept = (sum_y - result.slope * sum_x) / (double)n;

    /* Compute R-squared */
    double mean_y = sum_y / (double)n;
    double ss_tot = 0.0, ss_res = 0.0;
    for (size_t i = 0; i < n; i++) {
        double x    = (double)series->points[i].timestamp;
        double y    = (double)series->points[i].used_bytes;
        double y_hat = result.slope * x + result.intercept;
        ss_tot += (y - mean_y) * (y - mean_y);
        ss_res += (y - y_hat)  * (y - y_hat);
    }
    result.r_squared = (ss_tot > 1e-12) ? (1.0 - ss_res / ss_tot) : 1.0;

    /* Project fill time using last total_bytes */
    uint32_t capacity = series->points[n - 1].total_bytes;
    if (result.slope > 1e-12 && capacity > 0) {
        double t_full = ((double)capacity - result.intercept) / result.slope;
        result.projected_full = (t_full > 0.0) ? (uint32_t)t_full : 0;
    }

    result.is_valid = 1;
    return result;
}

uint32_t flash_trend_project(const FlashTrendResult *result,
                             uint64_t future_timestamp)
{
    if (!result || !result->is_valid) return 0;
    double projected = result->slope * (double)future_timestamp + result->intercept;
    if (projected < 0.0) return 0;
    return (uint32_t)projected;
}

void flash_trend_reset(FlashTrendSeries *series)
{
    if (!series) return;
    series->count = 0;
    memset(series->points, 0, sizeof(series->points));
}
