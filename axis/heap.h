#ifndef HEAP_H
#define HEAP_H

/*
 * I think this is really bad
 * and probably too slow for full-scale usage
 * TODO: more research on allocator logic
 */

#include <ultra64.h>
#include <string.h>
#include "stdint.h"
#include "printf.h"

#define block_size (0x1000)

void* g_heap_start;
uint32_t g_heap_size;
#define g_heap_end ((void*)(g_heap_start + g_heap_size))

typedef struct
{
    void* next;
    void* previous;
    uint32_t size;
} block_header_t;

typedef struct
{
    block_header_t* first_free;
    block_header_t* last_free;
} allocator_t;

allocator_t g_allocator;

void g_heap_construct(void* start, uint32_t size) {
    uint32_t index;

    g_heap_start = start;
    g_heap_size = size;

    for (index = 0; index < size / block_size; index++) {
        if (index != (size / block_size) - 1) ((block_header_t*)((uint8_t*)start + (block_size * index)))->next = ((uint8_t*)start + (block_size * (index + 1)));
        else {
            ((block_header_t*)((uint8_t*)start + (block_size * index)))->next = 0;
            g_allocator.last_free = (block_header_t*)((uint8_t*)start + (block_size * index));
        }

        if (index != 0) ((block_header_t*)((uint8_t*)start + (block_size * index)))->previous = ((uint8_t*)start + (block_size * (index - 1)));
        else {
            ((block_header_t*)((uint8_t*)start + (block_size * index)))->previous = 0;
            g_allocator.first_free = (block_header_t*)((uint8_t*)start + (block_size * index));
        }

        ((block_header_t*)((uint8_t*)start + (block_size * index)))->size = 0;
    }
}

void* malloc(uint32_t size) {
    uint32_t i;
    uint32_t required_blocks = ceil(size / block_size);
    block_header_t* block = g_allocator.first_free;
    block_header_t* next_block = NULL;

    if (size == 0) {
        printf("Tried to malloc 0??\n");
        return NULL;
    }

    while (block != 0) {
        if (block->size == 0) {
            if (required_blocks = 1) {
                block->size = size;
                return (uint8_t*)block + sizeof(block_header_t);
            }
            else {
                next_block = (block_header_t*)block->next;
                for (i = 0; i < required_blocks - 1; i++) {
                    next_block = (block_header_t*)next_block->next;
                    if (next_block->size != 0) {
                        block = next_block;
                        goto malloc_forloop_exit;
                    }
                }

                next_block = block;
                for (i = 0; i < required_blocks - 1; i++) {
                    next_block->size = block_size;
                    if (i == required_blocks - 1) next_block->size = (size - (block_size * i - 1));
                    next_block = (block_header_t*)next_block->next;
                }

                return (uint8_t*)block + sizeof(block_header_t);
            }
        }
        malloc_forloop_exit:
        block = (block_header_t*)block->next;
    }

    printf("Failed to malloc memory of size %X!\n", size);
    return block;
}

void free(void* block) {
    block_header_t* bh = ((block_header_t*)((uint8_t*)block - sizeof(block_header_t)));
    uint32_t required_blocks = ceil(bh->size / block_size);
    int i;

    for (i = 0; i < required_blocks; i++) {
        bh->size = 0;
        bh = bh->next;
    }
}

#endif

