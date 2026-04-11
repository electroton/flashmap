#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/flashcluster.h"
#include "../src/flashregion.h"

static FlashRegion make_region(const char *name, uint32_t addr, uint32_t size) {
    FlashRegion r;
    strncpy(r.name, name, sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';
    r.address = addr;
    r.size = size;
    return r;
}

static void test_cluster_empty_input(void) {
    FlashClusterResult res = flash_cluster_by_proximity(NULL, 0, 256);
    assert(res.count == 0);
    assert(res.clusters == NULL);
    printf("PASS: test_cluster_empty_input\n");
}

static void test_cluster_single_region(void) {
    FlashRegion r = make_region("boot", 0x0000, 0x1000);
    FlashClusterResult res = flash_cluster_by_proximity(&r, 1, 256);
    assert(res.count == 1);
    assert(res.clusters[0].count == 1);
    assert(res.clusters[0].start_address == 0x0000);
    assert(res.clusters[0].end_address == 0x1000);
    flash_cluster_result_free(&res);
    printf("PASS: test_cluster_single_region\n");
}

static void test_cluster_two_close_regions(void) {
    FlashRegion regions[2];
    regions[0] = make_region("text", 0x0000, 0x1000);
    regions[1] = make_region("data", 0x1080, 0x0200);
    FlashClusterResult res = flash_cluster_by_proximity(regions, 2, 256);
    assert(res.count == 1);
    assert(res.clusters[0].count == 2);
    assert(res.clusters[0].start_address == 0x0000);
    assert(res.clusters[0].end_address == 0x1280);
    flash_cluster_result_free(&res);
    printf("PASS: test_cluster_two_close_regions\n");
}

static void test_cluster_two_far_regions(void) {
    FlashRegion regions[2];
    regions[0] = make_region("text", 0x0000, 0x1000);
    regions[1] = make_region("bss",  0x8000, 0x0400);
    FlashClusterResult res = flash_cluster_by_proximity(regions, 2, 256);
    assert(res.count == 2);
    assert(res.clusters[0].count == 1);
    assert(res.clusters[1].count == 1);
    assert(res.clusters[1].start_address == 0x8000);
    flash_cluster_result_free(&res);
    printf("PASS: test_cluster_two_far_regions\n");
}

static void test_cluster_total_and_used_size(void) {
    FlashRegion regions[3];
    regions[0] = make_region("a", 0x0000, 0x0500);
    regions[1] = make_region("b", 0x0500, 0x0300);
    regions[2] = make_region("c", 0x0900, 0x0100);
    FlashClusterResult res = flash_cluster_by_proximity(regions, 3, 0x200);
    assert(res.count == 1);
    assert(flash_cluster_total_size(&res.clusters[0]) == 0x0A00);
    assert(flash_cluster_used_size(&res.clusters[0]) == 0x0900);
    flash_cluster_result_free(&res);
    printf("PASS: test_cluster_total_and_used_size\n");
}

static void test_cluster_unsorted_input(void) {
    FlashRegion regions[3];
    regions[0] = make_region("c", 0x2000, 0x0100);
    regions[1] = make_region("a", 0x0000, 0x0800);
    regions[2] = make_region("b", 0x0900, 0x0200);
    FlashClusterResult res = flash_cluster_by_proximity(regions, 3, 0x200);
    assert(res.count == 2);
    assert(res.clusters[0].start_address == 0x0000);
    assert(res.clusters[1].start_address == 0x2000);
    flash_cluster_result_free(&res);
    printf("PASS: test_cluster_unsorted_input\n");
}

int main(void) {
    test_cluster_empty_input();
    test_cluster_single_region();
    test_cluster_two_close_regions();
    test_cluster_two_far_regions();
    test_cluster_total_and_used_size();
    test_cluster_unsorted_input();
    printf("All flashcluster tests passed.\n");
    return 0;
}
