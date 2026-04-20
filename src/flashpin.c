#include "flashpin.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FlashPinSet *flashpin_create(void) {
    FlashPinSet *ps = calloc(1, sizeof(FlashPinSet));
    if (!ps) return NULL;
    ps->count = 0;
    ps->capacity = 8;
    ps->pins = calloc(ps->capacity, sizeof(FlashPin));
    if (!ps->pins) { free(ps); return NULL; }
    return ps;
}

void flashpin_destroy(FlashPinSet *ps) {
    if (!ps) return;
    free(ps->pins);
    free(ps);
}

int flashpin_add(FlashPinSet *ps, const char *name, uint32_t address, FlashPinType type) {
    if (!ps || !name) return -1;
    if (ps->count >= ps->capacity) {
        size_t new_cap = ps->capacity * 2;
        FlashPin *tmp = realloc(ps->pins, new_cap * sizeof(FlashPin));
        if (!tmp) return -1;
        ps->pins = tmp;
        ps->capacity = new_cap;
    }
    FlashPin *pin = &ps->pins[ps->count];
    strncpy(pin->name, name, FLASH_PIN_NAME_MAX - 1);
    pin->name[FLASH_PIN_NAME_MAX - 1] = '\0';
    pin->address = address;
    pin->type = type;
    pin->locked = false;
    ps->count++;
    return 0;
}

int flashpin_remove(FlashPinSet *ps, const char *name) {
    if (!ps || !name) return -1;
    for (size_t i = 0; i < ps->count; i++) {
        if (strcmp(ps->pins[i].name, name) == 0) {
            if (ps->pins[i].locked) return -2;
            memmove(&ps->pins[i], &ps->pins[i + 1],
                    (ps->count - i - 1) * sizeof(FlashPin));
            ps->count--;
            return 0;
        }
    }
    return -1;
}

FlashPin *flashpin_find(FlashPinSet *ps, const char *name) {
    if (!ps || !name) return NULL;
    for (size_t i = 0; i < ps->count; i++) {
        if (strcmp(ps->pins[i].name, name) == 0)
            return &ps->pins[i];
    }
    return NULL;
}

FlashPin *flashpin_find_by_address(FlashPinSet *ps, uint32_t address) {
    if (!ps) return NULL;
    for (size_t i = 0; i < ps->count; i++) {
        if (ps->pins[i].address == address)
            return &ps->pins[i];
    }
    return NULL;
}

int flashpin_lock(FlashPinSet *ps, const char *name) {
    FlashPin *pin = flashpin_find(ps, name);
    if (!pin) return -1;
    pin->locked = true;
    return 0;
}

int flashpin_unlock(FlashPinSet *ps, const char *name) {
    FlashPin *pin = flashpin_find(ps, name);
    if (!pin) return -1;
    pin->locked = false;
    return 0;
}

size_t flashpin_count(const FlashPinSet *ps) {
    return ps ? ps->count : 0;
}
