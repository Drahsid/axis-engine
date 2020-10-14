#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "app_context.h"
#include "engine/heap.h"
#include "engine/filesystem.h"
#include "math/vector.h"
#include "engine/graphics.h"
#include "examples/snake/overlay/snake_player.h"
#include "examples/snake/overlay/snake_player_c.h"

void mainstep_default(app_context_t* app) {
    if (app->graphics_context.frame_count % 600 == 0) {
        printf("dt mainstep %f\ndt mainproc %f\n", app->main_time, app->mainproc_time);
    }

    // TODO: if there is a way to do this on demand instead if in a separate thread, I want to find it
    // Process controllers
    if (app->input_context.controller_valid && !app->input_context.controller_read) {
        osContStartReadData(&app->graphics_context.rsp_message_queue);
        app->input_context.controller_read = 1;
    }

    // Process camera
    vec3f_t* position = &app->camera.actor.position;
    vec3f_t* forward = &app->camera.forward;
    vec3f_t* right = &app->camera.right;
    vec3f_t* up = &app->camera.up;
    vec3f_t temp = vec3f_zero;

    *forward = vec3f_forward;
    *right = vec3f_right;
    *up = vec3f_up;

    snake_player_step(app, &app->player);
}

#endif

