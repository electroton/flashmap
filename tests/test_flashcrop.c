#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "flashcrop.h"
#include "flashlayout.h"
#include "flashregion.h"

/* Helper: build a simple layout with N regions */
static FlashLayout *make_layout(void)
{
    FlashLayout *l = flash_layout_create();
    assert(l);

    FlashRegion regions[] = {
        { .name = "bootloader", .address = 0x0000, .size = 0x4000, .flags = 0 },
        { .name = "app",        .address = 0x4000, .size = 0x8000, .flags = 0 },
        { .name = "data",       .address = 0xC000, .size = 0x2000, .flags = 0 },
        { .name = "nvs",        .address = 0xE000, .size = 0x2000, .flags = 0 },
    };

    for (size_t i = 0; i < 4; i++)
        assert(flash_layout_add_region(l, &regions[i]) == 0);

    return l;
}

static void test_crop_fully_inside(void)
{
    FlashLayout *l = make_layout();
    FlashCropResult res;

    /* Window exactly covers app + data */
    int rc = flash_crop(l, 0x4000, 0x8000, CROP_MODE_EXCLUDE, &res);
    assert(rc == 0);
    assert(res.layout);
    assert(res.layout->count == 2);
    assert(res.excluded == 2); /* bootloader + nvs excluded */
    assert(res.trimmed  == 0);

    flash_crop_result_free(&res);
    flash_layout_free(l);
    printf("PASS: test_crop_fully_inside\n");
}

static void test_crop_exclude_partial(void)
{
    FlashLayout *l = make_layout();
    FlashCropResult res;

    /* Window starts in the middle of bootloader region */
    int rc = flash_crop(l, 0x2000, 0xA000, CROP_MODE_EXCLUDE, &res);
    assert(rc == 0);
    /* app (0x4000..0xC000) is fully inside; bootloader partial -> excluded */
    assert(res.layout->count == 1);
    assert(strcmp(res.layout->regions[0].name, "app") == 0);
    assert(res.excluded >= 1);
    assert(res.trimmed  == 0);

    flash_crop_result_free(&res);
    flash_layout_free(l);
    printf("PASS: test_crop_exclude_partial\n");
}

static void test_crop_trim_partial(void)
{
    FlashLayout *l = make_layout();
    FlashCropResult res;

    /* Window trims the start of app region */
    int rc = flash_crop(l, 0x6000, 0x8000, CROP_MODE_TRIM, &res);
    assert(rc == 0);
    /* app overlaps: trimmed to 0x6000..0xC000 (size 0x6000) */
    /* data (0xC000..0xE000) is fully inside */
    assert(res.trimmed >= 1);
    /* Verify trimmed region address and size */
    int found_trimmed = 0;
    for (size_t i = 0; i < res.layout->count; i++) {
        FlashRegion *r = &res.layout->regions[i];
        if (strcmp(r->name, "app") == 0) {
            assert(r->address == 0x6000);
            assert(r->size    == 0x6000);
            found_trimmed = 1;
        }
    }
    assert(found_trimmed);

    flash_crop_result_free(&res);
    flash_layout_free(l);
    printf("PASS: test_crop_trim_partial\n");
}

static void test_crop_empty_window(void)
{
    FlashLayout *l = make_layout();
    FlashCropResult res;

    /* Zero-size window should fail */
    int rc = flash_crop(l, 0x0000, 0, CROP_MODE_TRIM, &res);
    assert(rc == -1);

    flash_layout_free(l);
    printf("PASS: test_crop_empty_window\n");
}

static void test_crop_null_layout(void)
{
    FlashCropResult res;
    int rc = flash_crop(NULL, 0x0000, 0x10000, CROP_MODE_EXCLUDE, &res);
    assert(rc == -1);
    printf("PASS: test_crop_null_layout\n");
}

int main(void)
{
    test_crop_fully_inside();
    test_crop_exclude_partial();
    test_crop_trim_partial();
    test_crop_empty_window();
    test_crop_null_layout();
    printf("All flashcrop tests passed.\n");
    return 0;
}
