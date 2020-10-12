#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <ultra64.h>
#include "math/math_common.h"
#include "filesystem.h"
#include "graphics.h"
#include "input.h"
#include "heap.h"
#include "stdint.h"

extern char _codeSegmentEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];
extern char _filesystemSegmentRomStart[], _filesystemSegmentRomEnd[];

struct app_context_t;
typedef void (*step_func_t)(struct app_context_t*);
typedef void (*game_proc_t)(void*);

#define	STACKSIZE 0x2000
#define STACKSIZE_NUM (STACKSIZE / sizeof(uint64_t))

// TODO: figure out what to do with this
#define NUM_PI_MSGS (8)

enum thread_id {
	THREAD_IDLE,
	THREAD_MAIN,
	THREAD_DRAW,
	THREAD_CONT,
	THREAD_COUNT
};

typedef struct
{
    uint64_t* boot_stack;
    OSThread threads[THREAD_COUNT];
    uint64_t stacks[THREAD_COUNT][STACKSIZE_NUM];
    uint32_t memory_size;

    step_func_t main_step;
    step_func_t draw_step;
    step_func_t cont_step;

    filesystem_info_t filesystem;
    graphics_context_t graphics_context;
    input_context_t input_context;
    heap_t heap;

    OSMesgQueue pi_message_queue;
    OSMesg pi_messages[NUM_PI_MSGS];

    uint64_t main_ticks;
    uint64_t draw_ticks;
    uint64_t cont_ticks;

    // TODO: Create camera context v
    vec3f_t euler;
	vec3f_t orbit;
	vec3f_t position;
	vec3f_t velocity;
	vec3f_t forward;
	vec3f_t right;
	vec3f_t up;
	vec3i_t vhj;
} app_context_t;

// Set up threads and basic info
void app_context_construct(app_context_t* app, uint64_t* boot_stack, game_proc_t idleproc, game_proc_t mainproc, game_proc_t drawproc, game_proc_t contproc, void* arg) {
    app->boot_stack = boot_stack;
    app->memory_size = osGetMemSize();
    printf("have memory size %X\n", app->memory_size);

    osCreateThread(&app->threads[THREAD_IDLE], THREAD_IDLE + 1, idleproc, arg, ((uint8_t*)&app->stacks[THREAD_IDLE][0]) + STACKSIZE, 10);
	osCreateThread(&app->threads[THREAD_MAIN], THREAD_MAIN + 1, mainproc, arg, ((uint8_t*)&app->stacks[THREAD_MAIN][0]) + STACKSIZE, 11);
	osCreateThread(&app->threads[THREAD_DRAW], THREAD_DRAW + 1, drawproc, arg, ((uint8_t*)&app->stacks[THREAD_DRAW][0]) + STACKSIZE, 12);
	osCreateThread(&app->threads[THREAD_CONT], THREAD_CONT + 1, contproc, arg, ((uint8_t*)&app->stacks[THREAD_CONT][0]) + STACKSIZE, 10);
    printf("Created threads\n");

    heap_construct(&app->heap, (void*)(0x80000000u + (app->memory_size / 2)), app->memory_size / 2);
    g_heap = &app->heap;
    printf("heap construct at %X with %X space\n", app->heap.start, app->heap.size);

    app->euler = vec_new(0, HPI, 0);
    app->orbit = vec3f_zero;
    app->position = vec_new(0, 0, -100.0f);
    app->velocity = vec3f_zero;
    app->forward = vec3f_forward;
    app->right = vec3f_right;
    app->up = vec3f_up;
    app->vhj = vec3i_zero;

    app->main_ticks = 0;
    app->draw_ticks = 0;
    app->cont_ticks = 0;
}

// Initialize/construct members
void app_context_initialize(app_context_t* app) {
    graphics_context_construct(&app->graphics_context);
    printf("gfx constructed\n");

    filesystem_info_construct(&app->filesystem, osCartRomInit(), _codeSegmentEnd, _filesystemSegmentRomStart);
    printf("filesystem constructed\n");

    input_context_construct(&app->input_context);
    printf("input constructed\n");
}

#endif

