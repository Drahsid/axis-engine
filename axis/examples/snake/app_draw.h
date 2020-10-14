#ifndef APP_DRAW_H
#define APP_DRAW_H

#include <ultra64.h>
#include <PR/gbi.h>
#include "app_context.h"
#include "math/math_common.h"
#include "engine/graphics.h"
#include "engine/printf.h"
#include "examples/snake/overlay/snake_player.h"
#include "examples/snake/overlay/snake_player_c.h"

void drawstep_default(app_context_t* app) {
    int index;

    if (app->graphics_context.frame_count % 600 == 0) {
        printf("dt drawstep %f\ndt drawproc %f\n", app->draw_time, app->drawproc_time);
    }

    guOrtho(&app->graphics_context.view.dynamic.projection,
            -(float)SCREEN_WD / 2.0f, (float)SCREEN_WD / 2.0f,
            -(float)SCREEN_HT / 2.0f, (float)SCREEN_HT / 2.0f,
            app->camera.near, app->camera.far, app->camera.scale);

    //guPositionLook(app->graphics_context.view.dynamic.viewing.m, &app->camera);
    guRotate(app->graphics_context.view.dynamic.viewing.m, 0, 0 , 0, 1.0f);

    graphics_context_reset(&app->graphics_context, _codeSegmentEnd);

    gDPPipeSync(app->graphics_context.glistp++);
    gDPSetFillColor(app->graphics_context.glistp++, GPACK_RGBA5551(238, 130, 238, 255));
    gDPFillRectangle(app->graphics_context.glistp++, 24, 24, SCREEN_WD - 24, SCREEN_HT - 24);

    gDPPipeSync(app->graphics_context.glistp++);
    gDPSetFillColor(app->graphics_context.glistp++, GPACK_RGBA5551(127, 255, 71, 255));
    gDPFillRectangle(app->graphics_context.glistp++,
                    (int)(app->food.position.x - 8.0f + ((float)SCREEN_WD / 2.0f)), (int)(app->food.position.y - 8.0f + ((float)SCREEN_HT / 2.0f)),
                    (int)(app->food.position.x + 8.0f + ((float)SCREEN_WD / 2.0f)), (int)(app->food.position.y + 8.0f + ((float)SCREEN_HT / 2.0f)));

    // draw body
    if (app->player.body_list) {
        for (index = 0; index < app->player.body_count; index++) {
            int ulx, uly, lrx, lry, color;

            if (index) color = 0xFFFFFFFF;
            else color = 0x808080FF;

            ulx = (int)((app->player.body_list[index]->position.x - 7.0f) + ((float)SCREEN_WD / 2.0f));
            uly = (int)((app->player.body_list[index]->position.y - 7.0f) + ((float)SCREEN_HT / 2.0f));
            lrx = (int)((app->player.body_list[index]->position.x + 7.0f) + ((float)SCREEN_WD / 2.0f));
            lry = (int)((app->player.body_list[index]->position.y + 7.0f) + ((float)SCREEN_HT / 2.0f));

            //printf("Drawing body index %d at coords (%d, %d)\n", index, app->player.body_list[index]->position.x, app->player.body_list[index]->position.y);

            gDPPipeSync(app->graphics_context.glistp++);
            gDPSetFillColor(app->graphics_context.glistp++, 0xFFFFFFFF);
            gDPFillRectangle(app->graphics_context.glistp++, ulx, uly, lrx, lry);
        }
    }

    graphics_context_end(&app->graphics_context);
    graphics_context_setup_tlist(&app->graphics_context);

    // perform tasks
    osWritebackDCache(&app->graphics_context.view.dynamic, sizeof(dynamic_t));
    osSpTaskStart(app->graphics_context.tlistp);

    app->graphics_context.frame_count++;
}

#endif

