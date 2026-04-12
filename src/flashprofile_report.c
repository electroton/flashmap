#include "flashprofile_report.h"
#include <stdio.h>
#include <string.h>

int flash_profile_report_to_buffer(const FlashProfile       *profile,
                                    const FlashProfileResult *results,
                                    int                       result_count,
                                    char                     *buf,
                                    int                       buf_size) {
    if (!profile || !results || !buf || buf_size <= 0) return -1;

    int offset = 0;
    int rem    = buf_size;
    int n;

    n = snprintf(buf + offset, rem, "Profile: %s\n", profile->name);
    if (n < 0 || n >= rem) return -1;
    offset += n; rem -= n;

    n = snprintf(buf + offset, rem, "Rules evaluated: %d\n", result_count);
    if (n < 0 || n >= rem) return -1;
    offset += n; rem -= n;

    int passed = 0;
    for (int i = 0; i < result_count; i++) {
        const char *status = results[i].passed ? "PASS" : "FAIL";
        n = snprintf(buf + offset, rem, "  [%s] %s\n", status, results[i].message);
        if (n < 0 || n >= rem) return -1;
        offset += n; rem -= n;
        if (results[i].passed) passed++;
    }

    n = snprintf(buf + offset, rem, "Summary: %d/%d passed\n", passed, result_count);
    if (n < 0 || n >= rem) return -1;
    offset += n;

    return offset;
}

bool flash_profile_report_to_file(const FlashProfile       *profile,
                                   const FlashProfileResult *results,
                                   int                       result_count,
                                   const char               *filepath) {
    char buf[4096];
    int  len = flash_profile_report_to_buffer(profile, results, result_count,
                                               buf, sizeof(buf));
    if (len < 0) return false;

    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    bool ok = (fwrite(buf, 1, (size_t)len, fp) == (size_t)len);
    fclose(fp);
    return ok;
}
