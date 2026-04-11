#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/flashalign.h"
#include "../src/flashlayout.h"
#include "../src/flashregion.h"

static FlashLayout make_layout(FlashRegion *regions, int count) {
    FlashLayout layout;
    layout.regions = regions;
    layout.region_count = count;
    return layout;
}

void test_is_aligned() {
    assert(flashalign_is_aligned(0x08000000, 256) == true);
    assert(flashalign_is_aligned(0x08000100, 256) == true);
    assert(flashalign_is_aligned(0x08000001, 256) == false);
    assert(flashalign_is_aligned(0x00000000, 4)   == true);
    assert(flashalign_is_aligned(0x00000003, 4)   == false);
    assert(flashalign_is_aligned(0x00000004, 4)   == true);
    printf("PASS: test_is_aligned\n");
}

void test_no_issues_when_aligned() {
    FlashRegion regions[] = {
        {"FLASH",  0x08000000, 0x00020000, 0},
        {"RAM",    0x20000000, 0x00008000, 0}
    };
    FlashLayout layout = make_layout(regions, 2);
    AlignReport report = flashalign_check(&layout, 256);
    assert(report.issue_count == 0);
    assert(report.error_count == 0);
    assert(report.warning_count == 0);
    flashalign_free_report(&report);
    printf("PASS: test_no_issues_when_aligned\n");
}

void test_detects_address_misalignment() {
    FlashRegion regions[] = {
        {"BOOTLOADER", 0x08000001, 0x00004000, 0}
    };
    FlashLayout layout = make_layout(regions, 1);
    AlignReport report = flashalign_check(&layout, 256);
    assert(report.issue_count == 1);
    assert(report.error_count == 1);
    assert(report.issues[0].severity == ALIGN_ERROR);
    assert(strcmp(report.issues[0].region_name, "BOOTLOADER") == 0);
    flashalign_free_report(&report);
    printf("PASS: test_detects_address_misalignment\n");
}

void test_detects_size_misalignment() {
    FlashRegion regions[] = {
        {"APP", 0x08004000, 0x00001234, 0}
    };
    FlashLayout layout = make_layout(regions, 1);
    AlignReport report = flashalign_check(&layout, 256);
    assert(report.issue_count == 1);
    assert(report.warning_count == 1);
    assert(report.issues[0].severity == ALIGN_WARNING);
    flashalign_free_report(&report);
    printf("PASS: test_detects_size_misalignment\n");
}

void test_multiple_regions_mixed() {
    FlashRegion regions[] = {
        {"OK",    0x08000000, 0x00001000, 0},
        {"BAD1",  0x08001001, 0x00001000, 0},
        {"BAD2",  0x08002000, 0x00000001, 0}
    };
    FlashLayout layout = make_layout(regions, 3);
    AlignReport report = flashalign_check(&layout, 256);
    assert(report.issue_count == 2);
    assert(report.error_count == 1);
    assert(report.warning_count == 1);
    flashalign_free_report(&report);
    printf("PASS: test_multiple_regions_mixed\n");
}

void test_null_layout() {
    AlignReport report = flashalign_check(NULL, 256);
    assert(report.issue_count == 0);
    assert(report.issues == NULL);
    printf("PASS: test_null_layout\n");
}

int main(void) {
    test_is_aligned();
    test_no_issues_when_aligned();
    test_detects_address_misalignment();
    test_detects_size_misalignment();
    test_multiple_regions_mixed();
    test_null_layout();
    printf("All flashalign tests passed.\n");
    return 0;
}
