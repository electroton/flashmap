#include "flashpin_report.h"
#include <stdio.h>

static const char *pin_type_str(FlashPinType type) {
    switch (type) {
        case FLASH_PIN_ENTRY:    return "entry";
        case FLASH_PIN_BOUNDARY: return "boundary";
        case FLASH_PIN_MARKER:   return "marker";
        default:                 return "unknown";
    }
}

void flashpin_report_print_pin(const FlashPin *pin, FILE *out) {
    if (!pin || !out) return;
    fprintf(out, "  %-24s  0x%08X  %-10s  %s\n",
            pin->name,
            pin->address,
            pin_type_str(pin->type),
            pin->locked ? "[locked]" : "");
}

void flashpin_report_print(const FlashPinSet *ps, FILE *out) {
    if (!ps || !out) return;
    fprintf(out, "Flash Pin Report\n");
    fprintf(out, "================\n");
    fprintf(out, "  %-24s  %-10s  %-10s  %s\n",
            "Name", "Address", "Type", "Flags");
    fprintf(out, "  %s\n",
            "--------------------------------------------------------");
    for (size_t i = 0; i < ps->count; i++) {
        flashpin_report_print_pin(&ps->pins[i], out);
    }
    fprintf(out, "\nTotal pins: %zu\n", ps->count);
}
