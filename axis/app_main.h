#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "app_context.h"
#include "heap.h"
#include "filesystem.h"
#include "math/vector.h"
#include "graphics.h"

void mainstep_default(app_context_t* app) {
    if (app->main_ticks == 0) {
        char* sample_text = (char*)filesystem_info_alloc_and_read_file(&app->filesystem, "/data/storytime/story.txt");
        printf("%s\n\n", sample_text);
        free(sample_text);
    }

    // TODO: if there is a way to do this on demand instead if in a separate thread, I want to find it
    // Process controllers
    if (app->input_context.controller_valid && !app->input_context.controller_read) {
        osContStartReadData(&app->graphics_context.rsp_message_queue);
        app->input_context.controller_read = 1;
    }

    // Process camera
    vec3f_t* euler =  &app->euler;
    vec3f_t* orbit = &app->orbit;
    vec3f_t* position = &app->position;
    vec3f_t* velocity = &app->velocity;
    vec3f_t* forward = &app->forward;
    vec3f_t* right = &app->right;
    vec3f_t* up = &app->up;
    vec3i_t* vhj = &app->vhj;

    *vhj = vec3i_zero;

    euler->x += app->input_context.controller[0].stick_now.y * 0.0725f;
    euler->y += app->input_context.controller[0].stick_now.x * 0.0725f;

    // constrain and preserve
    if (euler->x > DTORF(89.0f)) euler->x = DTORF(89.0f);
    if (euler->x < DTORF(-89.0f)) euler->x = DTORF(-89.0f);
    if (euler->y > MATHF_TAU) euler->y -= MATHF_TAU;
    if (euler->y < MATHF_TAU) euler->y += MATHF_TAU;

    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CUP].state >= BUTTON_STATE_PRESSED) vhj->x += 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CDOWN].state >= BUTTON_STATE_PRESSED) vhj->x -= 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CRIGHT].state >= BUTTON_STATE_PRESSED) vhj->y -= 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CLEFT].state >= BUTTON_STATE_PRESSED) vhj->y += 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_RT].state >= BUTTON_STATE_PRESSED) vhj->z += 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_LT].state >= BUTTON_STATE_PRESSED) vhj->z -= 1;

    // calculate the euler orbit position based on pitch & yaw
    *orbit = vec_new(
        cosf(euler->x) * cosf(euler->y),
        -sinf(euler->x),
        cosf(euler->x) * sinf(euler->y));

    // which just so happens to also be our forward vector
    forward = orbit;

    // cross it with whatever is the last up vector
    *right = vec3f_cross(forward, up);

    // normalize
    vec3f_normalize_assignment(forward);
    vec3f_normalize_assignment(right);

    // convert to desired dir and check magnitude then normalize
    *velocity =	vec3f_add(
                vec3f_multiplyf(*forward, vhj->x),
                vec3f_multiplyf(*right, vhj->y));

    if ((up->x = vec3f_square_magnitude_p(velocity)) > 0) {
        vec3f_dividef_assignment(velocity, sqrtf(up->x));
    }

    // same for up and down
    if (vhj->z != 0) {
        *up = vec3f_cross(right, forward);

        if ((right->x = vec3f_square_magnitude_p(up)) != 0) {
            vec3f_dividef_assignment(up, sqrtf(right->x));
            *right = vec3f_multiplyf(*up, vhj->z * 5.0f);
            vec3f_add_assignment(position, right);
        }
    }

    // move
    *right = vec3f_multiplyf(*velocity, 5.0f);
    vec3f_add_assignment(position, right);

    *up = vec3f_up;
}

#endif

