#ifndef APP_DRAW_H
#define APP_DRAW_H

#include <ultra64.h>
#include "app_context.h"
#include "math/math_common.h"
#include "engine/graphics.h"
#include "engine/printf.h"

void drawstep_default(app_context_t* app) {
    if (app->graphics_context.frame_count % 600 == 0) {
        printf("dt drawstep %f\ndt drawproc %f\n", app->draw_time, app->drawproc_time);
    }

    guPerspective(&app->graphics_context.view.dynamic.projection, &app->camera.persp_norm, app->camera.fovy, app->camera.aspect, app->camera.near, app->camera.far, app->camera.scale);
    gSPPerspNormalize(app->graphics_context.glistp++, app->camera.persp_norm);

    guPositionLook(app->graphics_context.view.dynamic.viewing.m, &app->camera);

    Vtx* tri_vtx = malloc(sizeof(Vtx) * 4);
    tri_vtx[0] = (Vtx){ -64,  64, 0, 0, 0, 0, 0x40, 0x10, 0x40, 0xFF };
    tri_vtx[1] = (Vtx){  64,  64, 0, 0, 0, 0, 0x40, 0x40, 0x40, 0xFF };
    tri_vtx[2] = (Vtx){  64, -64, 0, 0, 0, 0, 0x40, 0x40, 0x10, 0xFF };
    tri_vtx[3] = (Vtx){ -64, -64, 0, 0, 0, 0, 0x10, 0x40, 0x40, 0xFF };

    Gfx shadetri_dl[] = {
        gsSPMatrix(osVirtualToPhysical(&(app->graphics_context.view.dynamic.projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH),
        gsSPMatrix(osVirtualToPhysical(&(app->graphics_context.view.dynamic.viewing)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH),
        gsDPPipeSync(),
        gsDPSetCycleType(G_CYC_1CYCLE),
        gsDPSetRenderMode(G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2),
        gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPVertex(&(tri_vtx[0]), 4, 0),
        gsSP1Triangle(0, 1, 2, 0),
        gsSP1Triangle(0, 2, 3, 0),
        gsSPEndDisplayList()
    };

    graphics_context_reset(&app->graphics_context, _codeSegmentEnd);

    // this would be where we actually draw stuff
    gSPDisplayList(app->graphics_context.glistp++, shadetri_dl);

    graphics_context_end(&app->graphics_context);
    graphics_context_setup_tlist(&app->graphics_context);

    // perform tasks
    osWritebackDCache(&app->graphics_context.view.dynamic, sizeof(dynamic_t));
    osSpTaskStart(app->graphics_context.tlistp);

    free(tri_vtx);

    app->graphics_context.frame_count++;
}

#endif

