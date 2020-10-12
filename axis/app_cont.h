#ifndef APP_CONT_H
#define APP_CONT_H

#include "app_context.h"
#include "input.h"
#include "graphics.h"
#include "printf.h"

void contstep_default(app_context_t* app) {
    if (app->graphics_context.frame_count % 60 == 0) {
        printf("dt contstep %f\ndt contproc %f\n", app->cont_time, app->contproc_time);
    }

    input_context_step(&app->input_context);

    if (app->input_context.controller[0].buttons[BUTTON_INDEX_A].state == BUTTON_STATE_PRESSED) {
        printf("A button pressed!\n");
    }

    if (app->input_context.controller[0].buttons[BUTTON_INDEX_A].state == BUTTON_STATE_DOWN) {
        printf("A button is down!\n");
    }

    if (app->input_context.controller[0].buttons[BUTTON_INDEX_A].state == BUTTON_STATE_RELEASED) {
        printf("A button released!\n");
    }

    if (app->input_context.controller[0].stick_now.x != app->input_context.controller[0].stick_before.x) {
        // being able to printf so fast lags emulator if you start treating the stick like mario party lol
        //printf("Stick moved horizontally! new %f - old %f = %f\n", app->input_context.controller[0].stick_now.x, app->input_context.controller[0].stick_before.x, app->input_context.controller[0].stick_now.x - app->input_context.controller[0].stick_before.x);
    }
}

#endif

