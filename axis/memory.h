#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"

void memcpy(void* dest, void* src, uint64_t length) {
    char* d = dest;
    const char* s = src;
    while (length--) *d++ = *s++;
}

#endif