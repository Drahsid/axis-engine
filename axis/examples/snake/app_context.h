#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <ultra64.h>
#include "math/math_common.h"
#include "engine/filesystem.h"
#include "engine/graphics.h"
#include "engine/input.h"
#include "engine/heap.h"
#include "engine/entity.h"
#include "engine/overlay/camera.h"
#include "examples/snake/overlay/snake_player.h"
#include "stdint.h"

extern char _codeSegmentEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];
extern char _filesystemSegmentRomStart[], _filesystemSegmentRomEnd[];

struct app_context_t;
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

    double main_time;
    double draw_time;
    double cont_time;

    double mainproc_time;
    double drawproc_time;
    double contproc_time;

    double total_time;
    snake_player_t player;
    actor_t food;

    uint32_t actor_count;
    actor_t** actor_list;

    camera_t camera;
} app_context_t;

#include "examples/snake/overlay/snake_player_c.h"

// Set up threads and basic info
void app_context_construct(app_context_t* app, uint64_t* boot_stack, game_proc_t idleproc, game_proc_t mainproc, game_proc_t drawproc, game_proc_t contproc, void* arg) {
    uint32_t fpstat;
    fpstat = __osGetFpcCsr();
	fpstat |= FPCSR_EZ;
    __osSetFpcCsr(fpstat);

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

    app->main_time = 0;
    app->draw_time = 0;
    app->cont_time = 0;
}

// Initialize/construct members
void app_context_initialize(app_context_t* app) {
    graphics_context_construct(&app->graphics_context);
    printf("gfx constructed\n");

    filesystem_info_construct(&app->filesystem, osCartRomInit(), _codeSegmentEnd, _filesystemSegmentRomStart);
    printf("filesystem constructed\n");

    input_context_construct(&app->input_context);
    printf("input constructed\n");

    camera_construct(&app->camera);

    snake_player_construct(app, &app->player);
    app->food.position = vec3f_zero;
}

app_context_actor_push(app_context_t* app, actor_t* actor) {
    int index = app->actor_count;
    app->actor_count++;

    if (app->actor_count == 0) app->actor_list = malloc(sizeof(actor_t*) * app->actor_count);
    else app->actor_list = realloc(app->actor_list, sizeof(actor_t*) * app->actor_count);

    app->actor_list[index] = actor;
}

app_context_actor_remove(app_context_t* app, actor_t* actor) {
    int index;

    for (index = 0; index < app->actor_count; index++) {
        if (app->actor_list[index] == actor) {
            if (app->actor_count > 1) {
                free(app->actor_list[index]);
                memcpy(&app->actor_list[index + 1], &app->actor_list[app->actor_count - 1], sizeof(actor_t*) * (((app->actor_count - 1) - (index + 1))));
                app->actor_list[app->actor_count - 1] = 0;
                app->actor_count--;
                break;
            }
            else {
                app->actor_count--;
                free(app->actor_list);
            }
        }
    }
}

app_context_deconstruct(app_context_t* app) {
    int index;
    for (index = 0; index < app->actor_count; index++) {
        free(app->actor_list[index]);
    }

    free(app->actor_list);

    snake_player_deconstruct(app, &app->player);
}

#endif

