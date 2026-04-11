#include "flashexport.h"
#include "flashregion.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

static int write_csv(const FlashLayout *layout, FILE *out,
                     const ExportOptions *opts)
{
    if (opts->include_header) {
        fprintf(out, "name,start,size,used,free,type\n");
    }

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        uint32_t free_bytes = r->size - r->used;
        fprintf(out, "%s,0x%08X,%u,%u,%u,%s\n",
                r->name,
                r->start,
                r->size,
                r->used,
                free_bytes,
                r->type == REGION_ROM ? "ROM" :
                r->type == REGION_RAM ? "RAM" : "OTHER");
    }
    return 0;
}

static int write_json(const FlashLayout *layout, FILE *out,
                      const ExportOptions *opts)
{
    const char *ind  = opts->pretty_print ? "  "  : "";
    const char *ind2 = opts->pretty_print ? "    " : "";
    const char *nl   = opts->pretty_print ? "\n"  : "";

    fprintf(out, "{%s", nl);
    fprintf(out, "%s\"regions\": [%s", ind, nl);

    for (int i = 0; i < layout->region_count; i++) {
        const FlashRegion *r = &layout->regions[i];
        uint32_t free_bytes = r->size - r->used;
        const char *comma = (i < layout->region_count - 1) ? "," : "";

        fprintf(out, "%s{%s", ind2, nl);
        fprintf(out, "%s%s\"name\": \"%s\",%s", ind2, ind, r->name, nl);
        fprintf(out, "%s%s\"start\": %u,%s", ind2, ind, r->start, nl);
        fprintf(out, "%s%s\"size\": %u,%s", ind2, ind, r->size, nl);
        fprintf(out, "%s%s\"used\": %u,%s", ind2, ind, r->used, nl);
        fprintf(out, "%s%s\"free\": %u,%s", ind2, ind, free_bytes, nl);
        fprintf(out, "%s%s\"type\": \"%s\"%s",
                ind2, ind,
                r->type == REGION_ROM ? "ROM" :
                r->type == REGION_RAM ? "RAM" : "OTHER",
                nl);
        fprintf(out, "%s}%s%s", ind2, comma, nl);
    }

    fprintf(out, "%s]%s", ind, nl);
    fprintf(out, "}%s", nl);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

ExportOptions flashexport_default_options(ExportFormat format)
{
    ExportOptions opts;
    opts.format         = format;
    opts.include_header = 1;
    opts.pretty_print   = 1;
    return opts;
}

int flashexport_write(const FlashLayout *layout, FILE *out,
                      const ExportOptions *options)
{
    if (!layout || !out || !options) return -1;

    switch (options->format) {
        case EXPORT_FORMAT_CSV:  return write_csv(layout, out, options);
        case EXPORT_FORMAT_JSON: return write_json(layout, out, options);
        default: return -1;
    }
}

int flashexport_to_file(const FlashLayout *layout, const char *path,
                        const ExportOptions *options)
{
    if (!path) return -1;
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    int rc = flashexport_write(layout, f, options);
    fclose(f);
    return rc;
}
