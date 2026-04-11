#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/flashcluster.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

/* Helper: build a simple layout with n regions */
static FlashLayout *make_layout(void) {
    FlashLayout *layout = flash_layout_create();
    assert(layout != NULL);

    /* Cluster A: two regions close together (gap = 0x10) */
    FlashRegion r1 = { .name = ".text",  .start = 0x08000000, .size = 0x1000 };
    FlashRegion r2 = { .name = ".rodata",.start = 0x08001010, .size = 0x0500 };

    /* Cluster B: one isolated region far away */
    FlashRegion r3 = { .name = ".data",  .start = 0x20000000, .size = 0x0200 };

    /* Cluster A continued: another region just after r2 (gap = 0x00) */
    FlashRegion r4 = { .name = ".bss",   .start = 0x08001510, .size = 0x0300 };

    flash_layout_add_region(layout, &r1);
    flash_layout_add_region(layout, &r2);
    flash_layout_add_region(layout, &r3);
    flash_layout_add_region(layout, &r4);

    return layout;
}

static void test_cluster_basic(void) {
    FlashLayout *layout = make_layout();
    FlashClusterResult result;
    memset(&result, 0, sizeof(result));

    /* Gap threshold of 0x100 should group r1, r2, r4 together and r3 alone */
    int ret = flash_cluster_regions(layout, 0x100, &result);
    assert(ret == 0);
    assert(result.count == 2);

    /* Cluster with 3 regions should span from r1.start to r4.end */
    int found_flash = 0, found_ram = 0;
    for (size_t i = 0; i < result.count; i++) {
        FlashCluster *c = &result.clusters[i];
        if (c->base == 0x08000000) {
            assert(c->count == 3);
            assert(c->end == 0x08001510 + 0x0300);
            found_flash = 1;
        } else if (c->base == 0x20000000) {
            assert(c->count == 1);
            assert(c->total_size == 0x0200);
            found_ram = 1;
        }
    }
    assert(found_flash);
    assert(found_ram);

    flash_cluster_result_free(&result);
    flash_layout_free(layout);
    printf("PASS: test_cluster_basic\n");
}

static void test_cluster_zero_threshold(void) {
    FlashLayout *layout = make_layout();
    FlashClusterResult result;
    memset(&result, 0, sizeof(result));

    /* Zero threshold: only exactly adjacent regions cluster */
    int ret = flash_cluster_regions(layout, 0, &result);
    assert(ret == 0);
    /* r1 and r2 have a gap of 0x10, so they are separate; r2 and r4 gap=0 */
    /* Expected: r1 alone, r2+r4 together, r3 alone => 3 clusters */
    assert(result.count == 3);

    flash_cluster_result_free(&result);
    flash_layout_free(layout);
    printf("PASS: test_cluster_zero_threshold\n");
}

static void test_cluster_empty_layout(void) {
    FlashLayout *layout = flash_layout_create();
    assert(layout != NULL);

    FlashClusterResult result;
    memset(&result, 0, sizeof(result));

    int ret = flash_cluster_regions(layout, 0x1000, &result);
    assert(ret == 0);
    assert(result.count == 0);
    assert(result.clusters == NULL);

    flash_cluster_result_free(&result);
    flash_layout_free(layout);
    printf("PASS: test_cluster_empty_layout\n");
}

int main(void) {
    test_cluster_basic();
    test_cluster_zero_threshold();
    test_cluster_empty_layout();
    printf("All flashcluster tests passed.\n");
    return 0;
}
