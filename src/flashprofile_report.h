#ifndef FLASHPROFILE_REPORT_H
#define FLASHPROFILE_REPORT_H

#include "flashprofile.h"

/*
 * Render a profile evaluation report to a caller-supplied buffer.
 * Returns the number of characters written (excluding NUL), or -1 on error.
 */
int flash_profile_report_to_buffer(const FlashProfile       *profile,
                                    const FlashProfileResult *results,
                                    int                       result_count,
                                    char                     *buf,
                                    int                       buf_size);

/*
 * Write a profile evaluation report to the given file path.
 * Returns true on success, false on failure.
 */
bool flash_profile_report_to_file(const FlashProfile       *profile,
                                   const FlashProfileResult *results,
                                   int                       result_count,
                                   const char               *filepath);

#endif /* FLASHPROFILE_REPORT_H */
