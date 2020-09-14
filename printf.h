#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include "inttypes.h"

#define stdout (0x80740000)

static char* printf_get_ptr(char* format) {
    int num_strings = *((uint32_t*)stdout);
    char* out = stdout + sizeof(uint32_t);
    for (int i = 0; i < num_strings; i++) {
        out += strlen(out) + 1;
    }

    return out;
}

#ifdef _LANGUAGE_C
#define printf(format, ...) sprintf(printf_get_ptr(format), format __VA_OPT__(,) __VA_ARGS__); \
*((uint32_t*)stdout) += 1;
#else
#define printf(format, ...)
#endif

#endif