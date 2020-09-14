#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <ultra64.h>
#include "matrix.h"
#include "vectortypes.h"
#include "inttypes.h"
#include "video.h"

#define	GLIST_LEN (8192)

typedef struct
{
    Mtx	projection;
	Mtx	modeling;
	Mtx	viewing;
    Mtx identity;
	Gfx	glist[GLIST_LEN];
} dynamic_t;


typedef struct {
    dynamic_t dyn;
	mtxf_t projection;
	mtxf_t viewing;
    mtxf_t identity;
    vec3f_t up;
    float fov;
    float aspect;
    float near;
    float far;
    float scale;
    uint16_t persp_norm;
} view_t;

typedef struct
{
    COLOR_DEPTH_TYPE framebuffer[2][SCREEN_WD * SCREEN_HT];
    COLOR_DEPTH_TYPE rsp_framebuffer[2][SCREEN_WD * SCREEN_HT];
    OSMesgQueue rdp_message_queue, sync_message_queue;
    OSMesg rdp_message_buffer, sync_message_buffer;
    view_t view;
    Vp viewport;
    uint8_t current_fb;
} graphics_context_t;

void view_t_construct(view_t* view) {
    int i;

    for (i = 0; i < 4; i++) {
        view->projection.mf[i][0] = 0;
        view->projection.mf[i][1] = 0;
        view->projection.mf[i][2] = 0;
        view->projection.mf[i][3] = 0;

        view->viewing.mf[i][0] = 0;
        view->viewing.mf[i][1] = 0;
        view->viewing.mf[i][2] = 0;
        view->viewing.mf[i][3] = 0;

        view->identity.mf[i][0] = 0;
        view->identity.mf[i][1] = 0;
        view->identity.mf[i][2] = 0;
        view->identity.mf[i][3] = 0;

        view->identity.mf[i][i] = 1;
    }

    vec3f_t_construct(&view->up, 0, 1, 0);
    view->fov = 1.797689f;
    view->aspect = SCREEN_WD / SCREEN_HT;
    view->near = 0.001f;
    view->far = 1000000.0f;
    view->scale = 1.0f;
}

void graphics_context_t_construct(graphics_context_t* context) {
    for (volatile uint32_t i; i < SCREEN_WD * SCREEN_HT; i++) {
        context->framebuffer[0][i] = i;
        context->rsp_framebuffer[0][i] = i;
        context->framebuffer[1][i] = i;
        context->rsp_framebuffer[1][i] = i;
    }

    view_t_construct(&context->view);

    context->viewport.vp.vscale[0] = SCREEN_WD * 2;
    context->viewport.vp.vscale[1] = SCREEN_HT * 2;
    context->viewport.vp.vscale[2] = context->view.far / 2;
    context->viewport.vp.vscale[3] = 0;

    context->viewport.vp.vtrans[0] = SCREEN_WD * 2;
    context->viewport.vp.vtrans[1] = SCREEN_HT * 2;
    context->viewport.vp.vtrans[2] = context->view.far / 2;
    context->viewport.vp.vtrans[3] = 0;

    context->current_fb = 0;
}

#endif

