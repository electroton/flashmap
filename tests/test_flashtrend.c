#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "../src/flashtrend.h"

static void test_init(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "FLASH0");
    assert(strcmp(s.region_name, "FLASH0") == 0);
    assert(s.count == 0);
    printf("  [PASS] test_init\n");
}

static void test_add_point(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "CODE");
    int rc = flash_trend_add_point(&s, 1000, 4096, 65536);
    assert(rc == 0);
    assert(s.count == 1);
    assert(s.points[0].timestamp   == 1000);
    assert(s.points[0].used_bytes  == 4096);
    assert(s.points[0].total_bytes == 65536);
    printf("  [PASS] test_add_point\n");
}

static void test_analyze_insufficient_data(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "DATA");
    flash_trend_add_point(&s, 0, 100, 1024);
    FlashTrendResult r = flash_trend_analyze(&s);
    assert(r.is_valid == 0);
    printf("  [PASS] test_analyze_insufficient_data\n");
}

static void test_analyze_linear_growth(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "CODE");
    /* Perfectly linear: used = 100 * t */
    for (uint64_t t = 1; t <= 5; t++) {
        flash_trend_add_point(&s, t, (uint32_t)(100 * t), 10000);
    }
    FlashTrendResult r = flash_trend_analyze(&s);
    assert(r.is_valid == 1);
    assert(fabs(r.slope - 100.0) < 1.0);
    assert(fabs(r.r_squared - 1.0) < 0.001);
    printf("  [PASS] test_analyze_linear_growth\n");
}

static void test_project(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "BOOT");
    flash_trend_add_point(&s, 0, 0,   8192);
    flash_trend_add_point(&s, 10, 200, 8192);
    flash_trend_add_point(&s, 20, 400, 8192);
    FlashTrendResult r = flash_trend_analyze(&s);
    assert(r.is_valid == 1);
    uint32_t proj = flash_trend_project(&r, 30);
    /* slope ~20, intercept ~0, project at t=30 => ~600 */
    assert(proj >= 550 && proj <= 650);
    printf("  [PASS] test_project\n");
}

static void test_project_invalid(void)
{
    FlashTrendResult r;
    memset(&r, 0, sizeof(r));
    r.is_valid = 0;
    uint32_t proj = flash_trend_project(&r, 9999);
    assert(proj == 0);
    printf("  [PASS] test_project_invalid\n");
}

static void test_reset(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "NVM");
    flash_trend_add_point(&s, 1, 100, 512);
    flash_trend_add_point(&s, 2, 200, 512);
    assert(s.count == 2);
    flash_trend_reset(&s);
    assert(s.count == 0);
    printf("  [PASS] test_reset\n");
}

static void test_add_point_overflow(void)
{
    FlashTrendSeries s;
    flash_trend_init(&s, "OVF");
    for (int i = 0; i < FLASH_TREND_MAX_POINTS; i++) {
        int rc = flash_trend_add_point(&s, (uint64_t)i, (uint32_t)(i * 10), 1024);
        assert(rc == 0);
    }
    int rc = flash_trend_add_point(&s, 9999, 9999, 1024);
    assert(rc == -1);
    assert(s.count == FLASH_TREND_MAX_POINTS);
    printf("  [PASS] test_add_point_overflow\n");
}

int main(void)
{
    printf("Running flashtrend tests...\n");
    test_init();
    test_add_point();
    test_analyze_insufficient_data();
    test_analyze_linear_growth();
    test_project();
    test_project_invalid();
    test_reset();
    test_add_point_overflow();
    printf("All flashtrend tests passed.\n");
    return 0;
}
