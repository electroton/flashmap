#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashstats.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* Helper: build a minimal layout with N pre-filled regions */
static FlashLayout make_layout(FlashRegion *regions, size_t count)
{
    FlashLayout l;
    l.regions = regions;
    l.count   = count;
    l.capacity = count;
    return l;
}

static void test_compute_null(void)
{
    FlashStats s;
    assert(flashstats_compute(NULL, &s) == -1);
    FlashLayout empty = {0};
    assert(flashstats_compute(&empty, &s) == -1);
    printf("PASS: test_compute_null\n");
}

static void test_compute_basic(void)
{
    FlashRegion regions[3] = {
        { .name = "bootloader", .start = 0x0000, .size = 0x4000, .capacity = 0x4000, .is_gap = 0 },
        { .name = "app",        .start = 0x4000, .size = 0x8000, .capacity = 0xC000, .is_gap = 0 },
        { .name = "gap",        .start = 0xC000, .size = 0x4000, .capacity = 0x4000, .is_gap = 1 },
    };
    FlashLayout layout = make_layout(regions, 3);
    FlashStats stats;
    assert(flashstats_compute(&layout, &stats) == 0);

    assert(stats.region_count    == 3);
    assert(stats.total_size      == 0x10000);
    assert(stats.used_size       == 0xC000);
    assert(stats.free_size       == 0x4000);
    assert(stats.largest_region  == 0x8000);
    assert(stats.smallest_region == 0x4000);
    assert(stats.min_address     == 0x0000);
    assert(stats.max_address     == 0x10000);
    assert(stats.address_span    == 0x10000);
    /* utilization: 0xC000 / 0x10000 = 75% */
    assert(stats.utilization_pct > 74.9 && stats.utilization_pct < 75.1);
    printf("PASS: test_compute_basic\n");
}

static void test_utilization_bar(void)
{
    FlashStats stats;
    memset(&stats, 0, sizeof(stats));
    stats.utilization_pct = 50.0;

    char buf[32];
    flashstats_utilization_bar(&stats, buf, 10);
    /* Expect "[#####.....]" */
    assert(buf[0]  == '[');
    assert(buf[11] == ']');
    assert(buf[12] == '\0');
    for (int i = 1; i <= 5; i++)  assert(buf[i] == '#');
    for (int i = 6; i <= 10; i++) assert(buf[i] == '.');
    printf("PASS: test_utilization_bar\n");
}

static void test_busiest_region(void)
{
    FlashRegion regions[2] = {
        { .name = "a", .start = 0x0000, .size = 0x1000, .capacity = 0x2000, .is_gap = 0 },
        { .name = "b", .start = 0x2000, .size = 0x1F00, .capacity = 0x2000, .is_gap = 0 },
    };
    FlashLayout layout = make_layout(regions, 2);
    const FlashRegion *busiest = flashstats_busiest_region(&layout);
    assert(busiest != NULL);
    assert(strcmp(busiest->name, "b") == 0);
    printf("PASS: test_busiest_region\n");
}

static void test_busiest_region_empty(void)
{
    FlashLayout empty = {0};
    assert(flashstats_busiest_region(&empty) == NULL);
    assert(flashstats_busiest_region(NULL)   == NULL);
    printf("PASS: test_busiest_region_empty\n");
}

int main(void)
{
    test_compute_null();
    test_compute_basic();
    test_utilization_bar();
    test_busiest_region();
    test_busiest_region_empty();
    printf("All flashstats tests passed.\n");
    return 0;
}
