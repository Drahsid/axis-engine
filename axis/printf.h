#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <string.h>
#include "stdint.h"

/* TODO: this is a pretty meh system
 * Ripped it from what I used for ModLoaderprint, made only a few changes
 * There are possible performance issues as the number of strings on the stack increases
 * If this ever becomes a problem, we could probably made a psuedo-object that also stores the pointer to the byte after the end of the previous message
 * which would save time by not searching for the end of the message stack
 */

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