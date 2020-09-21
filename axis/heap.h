#ifndef HEAP_H
#define HEAP_H

/* this took a lot of research
 * this hurts my dyslexia
 * but it works way well
 * I think?
*/

#include <ultra64.h>
#include <string.h>
#include "stdint.h"
#include "printf.h"

typedef struct heap_block_s {
    struct heap_block_s* free_last;
    struct heap_block_s* free_next;
    struct heap_block_s* used_last;
    struct heap_block_s* used_next;
    uint32_t used, free;
} heap_block_t;

typedef struct {
    heap_block_t* free_head;
    heap_block_t* free_tail;
    heap_block_t* used_head;
} heap_t;

#define HEAP_BLOCK_ALIGNMENT (16)
#define HEAP_ALIGN(lhs) ((((uint32_t)lhs) + HEAP_BLOCK_ALIGNMENT - 1) & ~(HEAP_BLOCK_ALIGNMENT - 1))
#define HEAP_BLOCK_HEADER_SIZE (HEAP_ALIGN(sizeof(heap_block_t)))

heap_t* g_heap;
void* g_heap_start;
uint32_t g_heap_size;
#define g_heap_end ((void*)(g_heap_start + g_heap_size))

void heap_construct(void* start, uint32_t size) {
    heap_block_t* block;

    // set up the globals
    g_heap_start = ((uint32_t)start) + sizeof(heap_t);
    g_heap_size = size - sizeof(heap_t);
    g_heap = start;

    // set up for first block
    block = (heap_block_t*)HEAP_ALIGN(g_heap_start);
    size -= ((uint32_t)block) - ((uint32_t)start);
    size = HEAP_ALIGN(size);

    block->free_last = NULL;
    block->free_next = NULL;
    block->used_last = NULL;
    block->used_next = NULL;

    block->used = HEAP_BLOCK_HEADER_SIZE;
    block->free = size - HEAP_BLOCK_HEADER_SIZE;

    g_heap->used_head = block;

    if (block->free) g_heap->free_head = block;
    else g_heap->free_head = NULL;
}

void* heap_alloc(heap_t* heap, uint32_t size) {
    heap_block_t* block;
    heap_block_t* new_block;

    size = HEAP_ALIGN(size);
    block = heap->free_head;

    while (block) {
        // does block have space?
        if (block->free >= size) {
            new_block = (heap_block_t*)(((uint32_t)block) + block->used);

            new_block->used = size;
            new_block->free = block->free - size;

            if (new_block->free) {
                new_block->free_last = block;
                new_block->free_next = block->free_next;

                if (block->free_next) new_block->free_next->free_last = new_block;
                block->free_next = new_block;

                if (heap->free_tail == block) heap->free_tail = new_block;
            }
            else {
                new_block->free_next = NULL;
                new_block->free_last = NULL;
            }

            block->free = 0;

            if (block->free_last) block->free_last->free_next = block->free_next;
            if (block->free_next) block->free_next->free_last = block->free_last;

            if (heap->free_head == block) heap->free_head = block->free_next;
            if (heap->free_tail == block) {
                if (block->free_next) heap->free_tail = block->free_next;
                else heap->free_tail = block->free_last;
            }

            block->free_last = NULL;
            block->free_next = NULL;

            new_block->used_last = block;
            new_block->used_next = block->used_next;

            if (block->used_next) new_block->used_next->used_last = new_block;
            block->used_next = new_block;

            return (void*)(((uint32_t)new_block) + HEAP_BLOCK_HEADER_SIZE);
        }

        block = block->free_next;
    }

    return NULL;
}

void heap_alloc_tail(heap_t* heap, uint32_t size) {
    heap_block_t* block;
    heap_block_t* new_block;

    size = HEAP_ALIGN(size);
    block = heap->free_tail;

    while (block) {
        // does block have space?
        if (block->free >= size) {
            new_block = (heap_block_t*)(((uint32_t)block) + block->used);

            new_block->used = size;
            new_block->free = block->free - size;

            if (new_block->free) {
                new_block->free_last = block;
                new_block->free_next = block->free_next;

                if (block->free_next) new_block->free_next->free_last = new_block;
                block->free_next = new_block;

                if (heap->free_tail == block) heap->free_tail = new_block;
            }
            else {
                new_block->free_next = NULL;
                new_block->free_last = NULL;
            }

            block->free = 0;

            if (block->free_last) block->free_last->free_next = block->free_next;
            if (block->free_next) block->free_next->free_last = block->free_last;

            if (heap->free_head == block) heap->free_head = block->free_next;
            if (heap->free_tail == block) {
                if (block->free_next) heap->free_tail = block->free_next;
                else heap->free_tail = block->free_last;
            }

            block->free_last = NULL;
            block->free_next = NULL;

            new_block->used_last = block;
            new_block->used_next = block->used_next;

            if (block->used_next) new_block->used_next->used_last = new_block;
            block->used_next = new_block;

            return (void*)(((uint32_t)new_block) + HEAP_BLOCK_HEADER_SIZE);
        }

        block = block->free_last;
    }

    return NULL;
}

void heap_free(heap_t* heap, void* address) {
    heap_block_t* block;
    heap_block_t* free_last;
    heap_block_t* used_last;

    block = (heap_block_t*)(((uint32_t)address) - HEAP_BLOCK_HEADER_SIZE);
    used_last = block->used_last;

    if (!used_last->free) {

        // find previous free block
        if (block->free_last) free_last = block->free_last;
        else {
            free_last = block->used_last;
            while (free_last) {
                if (free_last->free) break;
                free_last = free_last->used_last;
            }
        }

        // choose where to add to the free list
        if (free_last) {
            used_last->free_last = free_last;
            used_last->free_next = free_last->free_next;

            if (free_last->free_next) used_last->free_next->free_last = used_last;

            free_last->free_next = used_last;

            if (heap->free_tail == free_last) heap->free_tail = used_last;
        }
        else {
            used_last->free_next = heap->free_head;

            if (heap->free_head) heap->free_head->free_last = used_last;
            else heap->free_tail = used_last;
            heap->free_head = used_last;
        }
    }

    used_last->free += block->used + block->free;

    if (block->used_next) block->used_next->used_last = block->used_last;
    block->used_last->used_next = block->used_next;

    if (block->free) {
        if (block->free_last) block->free_last->free_next = block->free_next;
        if (block->free_next) block->free_next->free_last = block->free_last;

        if (heap->free_tail == block) heap->free_tail = block->free_last;
    }
}

void heap_wipe(heap_t* heap) {
    while (heap->used_head->used_next) {
        heap_free(heap, (void*)(((uint32_t)heap->used_head->used_next) + HEAP_BLOCK_HEADER_SIZE));
    }
}

uint32_t heap_get_free(heap_t* heap) {
    uint32_t total = 0;
    heap_block_t* block;

    block = heap->free_head;
    while(block) {
        total += block->free;
        block = block->free_next;
    }
}

#define malloc(size) heap_alloc(g_heap, size)
#define malloc_tail(size) heap_alloc_tail(g_heap, size)
#define free(address) heap_free(g_heap, address)

#endif

