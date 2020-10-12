#ifndef APP_DRAW_H
#define APP_DRAW_H

#include <ultra64.h>
#include "app_context.h"
#include "graphics.h"

extern Vtx shade_vtx[];

void drawstep_default(app_context_t* app) {
    Gfx shadetri_dl[] = {
        gsSPMatrix(osVirtualToPhysical(&(app->graphics_context.view.dynamic.projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH),
        gsSPMatrix(osVirtualToPhysical(&(app->graphics_context.view.dynamic.viewing)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH),
        gsDPPipeSync(),
        gsDPSetCycleType(G_CYC_1CYCLE),
        gsDPSetRenderMode(G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2),
        gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPVertex(&(shade_vtx[0]), 4, 0),
        gsSP1Triangle(0, 1, 2, 0),
        gsSP1Triangle(0, 2, 3, 0),
        gsSPEndDisplayList()
    };

    guPerspective(&app->graphics_context.view.dynamic.projection, &app->graphics_context.view.persp_norm, app->graphics_context.view.fov, app->graphics_context.view.aspect, app->graphics_context.view.near, app->graphics_context.view.far, app->graphics_context.view.scale);
    gSPPerspNormalize(app->graphics_context.glistp++, app->graphics_context.view.persp_norm);
    guLookAt(
        &app->graphics_context.view.dynamic.viewing,
        app->position.x, app->position.y, app->position.z, // eye
        app->position.x + app->orbit.x, app->position.y + app->orbit.y, app->position.z + app->orbit.z, // at / focus
        app->up.x, app->up.y, app->up.z // up
    );

    graphics_context_reset(&app->graphics_context, _codeSegmentEnd);

    // this would be where we actually draw stuff
    gSPDisplayList(app->graphics_context.glistp++, shadetri_dl);

    graphics_context_end(&app->graphics_context);
    graphics_context_setup_tlist(&app->graphics_context);

    // perform tasks
    osWritebackDCache(&app->graphics_context.view.dynamic, sizeof(dynamic_t));
    osSpTaskStart(app->graphics_context.tlistp);

    app->graphics_context.frame_count++;;
}

#endif

