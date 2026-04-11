#ifndef FLASHEXPORT_H
#define FLASHEXPORT_H

#include "flashlayout.h"
#include <stdio.h>

/*
 * flashexport.h - Export flash layout data to various formats
 *
 * Supports exporting parsed flash layout information to:
 *   - CSV format for spreadsheet analysis
 *   - JSON format for toolchain integration
 */

typedef enum {
    EXPORT_FORMAT_CSV,
    EXPORT_FORMAT_JSON
} ExportFormat;

typedef struct {
    ExportFormat format;
    int include_header;   /* CSV: include column headers */
    int pretty_print;     /* JSON: indent output */
} ExportOptions;

/**
 * Export a flash layout to the given file stream.
 *
 * @param layout   Pointer to a populated FlashLayout.
 * @param out      Output file stream (e.g. stdout or an open file).
 * @param options  Export configuration options.
 * @return 0 on success, -1 on error.
 */
int flashexport_write(const FlashLayout *layout, FILE *out,
                      const ExportOptions *options);

/**
 * Export a flash layout to a named file.
 *
 * @param layout   Pointer to a populated FlashLayout.
 * @param path     Destination file path.
 * @param options  Export configuration options.
 * @return 0 on success, -1 on error.
 */
int flashexport_to_file(const FlashLayout *layout, const char *path,
                        const ExportOptions *options);

/**
 * Return a default ExportOptions struct with sensible defaults.
 */
ExportOptions flashexport_default_options(ExportFormat format);

#endif /* FLASHEXPORT_H */
