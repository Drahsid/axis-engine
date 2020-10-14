#ifndef APP_CONT_H
#define APP_CONT_H

#include "app_context.h"
#include "engine/input.h"
#include "engine/graphics.h"
#include "engine/printf.h"

void contstep_default(app_context_t* app) {
    input_context_step(&app->input_context);
}

#endif

