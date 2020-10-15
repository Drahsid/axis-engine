#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <PR/gbi.h>
#include <PR/os.h>
#include <string.h>

#include "video.h"
#include "stdint.h"
#include "engine/overlay/camera.h"

typedef float mtxf_t[4][4];

typedef struct {
	Mtx	projection;
	Mtx	viewing;
    Mtx identity;
	Gfx	glist[GLIST_LEN];
} dynamic_t;

typedef struct {
    Vp viewport;
    dynamic_t dynamic;
} view_t;

typedef struct {
    OSMesgQueue rdp_message_queue, vsync_message_queue, rsp_message_queue;
    OSMesg rdp_message_buffer, vsync_message_buffer, rsp_message_buffer;
    OSTask tlist;

    COLOR_DEPTH_TYPE __attribute__((aligned(64))) framebuffer[2][SCREEN_WD * SCREEN_HT];

    view_t view;

    // TODO: Pointers and malloc
    Gfx rsp_init[5];
    Gfx rdp_init[16];
    Gfx clear_fb[7];

    Gfx* glistp;
    OSTask* tlistp;

    uint8_t current_fb;
    uint8_t ucode;
    uint64_t frame_count;
} __attribute__((aligned(16))) graphics_context_t;

void view_construct(view_t* view) {
    view->viewport.vp.vscale[0] = SCREEN_WD * 2;
    view->viewport.vp.vscale[1] = SCREEN_HT * 2;
    view->viewport.vp.vscale[2] = G_MAXZ / 2;
    view->viewport.vp.vscale[3] = 0;

    view->viewport.vp.vtrans[0] = SCREEN_WD * 2;
    view->viewport.vp.vtrans[1] = SCREEN_HT * 2;
    view->viewport.vp.vtrans[2] = G_MAXZ / 2;
    view->viewport.vp.vtrans[3] = 0;
}

void graphics_context_construct(graphics_context_t* context) {
    volatile uint32_t pixel = 0; // volatile to prevent memset optimization

    context->frame_count = 0;

    osCreateViManager(OS_PRIORITY_VIMGR);
    osViSetMode(&osViModeTable[OS_VI_NTSC_LPN1]);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);

    for (pixel; pixel < SCREEN_WD * SCREEN_HT; pixel++) {
        context->framebuffer[0][pixel] = 0;
        context->framebuffer[1][pixel] = 0;
    }

    OSTask tlist = {
        M_GFXTASK,
        OS_TASK_DP_WAIT,
        NULL,
        0,
        NULL,
        SP_UCODE_SIZE,
        NULL,
        SP_UCODE_DATA_SIZE,
        &dram_stack[0],
        SP_DRAM_STACK_SIZE8,
        &rdp_output[0],
        &rdp_output[0] + RDP_OUTPUT_LEN,
        NULL,
        0,
        NULL,
        0
    };

    memcpy(&context->tlist, &tlist, sizeof(OSTask));

    view_construct(&context->view);

    Gfx rsp_init[] = {
        gsSPViewport(&context->view.viewport),
        gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                            G_TEXTURE_GEN_LINEAR | G_LOD ),
        gsSPTexture(0, 0, 0, 0, G_OFF),
        gsSPSetGeometryMode(G_SHADE | G_ZBUFFER | G_SHADING_SMOOTH),
        gsSPEndDisplayList()
    };

    Gfx rdp_init[] = {
        gsDPSetCycleType(G_CYC_1CYCLE),
        gsDPPipelineMode(G_PM_1PRIMITIVE),
        gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT),
        gsDPSetTextureLOD(G_TL_TILE),
        gsDPSetTextureLUT(G_TT_NONE),
        gsDPSetTextureDetail(G_TD_CLAMP),
        gsDPSetTexturePersp(G_TP_NONE),
        gsDPSetTextureFilter(G_TF_BILERP),
        gsDPSetTextureConvert(G_TC_FILT),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetCombineKey(G_CK_NONE),
        gsDPSetAlphaCompare(G_AC_NONE),
        gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
        gsDPSetColorDither(G_CD_DISABLE),
        gsDPSetBlendMask(0xFF),
        gsDPPipeSync(),
        gsSPEndDisplayList()
    };

    Gfx clear_fb[] = {
        gsDPSetCycleType(G_CYC_FILL),
        gsDPSetDepthImage(g_zbuffer),
        gsDPSetColorImage(G_IM_FMT_RGBA, COLOR_DEPTH_SIZE, SCREEN_WD, g_rsp_framebuffer),
        gsDPSetFillColor(GPACK_RGBA5551(1, 0, 0, 0)),
        gsDPFillRectangle(0, 0, SCREEN_WD - 1, SCREEN_HT - 1),
        gsDPPipeSync(),
        gsSPEndDisplayList()
    };

    memcpy(context->rsp_init, rsp_init, sizeof(Gfx) * 5);
    memcpy(context->rdp_init, rdp_init, sizeof(Gfx) * 17);
    memcpy(context->clear_fb, clear_fb, sizeof(Gfx) * 7);

    context->glistp = &context->view.dynamic.glist[0];
    context->tlistp = &context->tlist;

    context->current_fb = 0;
    context->ucode = 0;

    osCreateMesgQueue(&context->rsp_message_queue, &context->rsp_message_buffer, 2);
    osSetEventMesg(OS_EVENT_SP, &context->rsp_message_queue, NULL);

    osCreateMesgQueue(&context->rdp_message_queue, &context->rdp_message_buffer, 2);
    osSetEventMesg(OS_EVENT_DP, &context->rdp_message_queue, NULL);

    osCreateMesgQueue(&context->vsync_message_queue, &context->vsync_message_buffer, 2);
    osViSetEvent(&context->vsync_message_queue, NULL, 1);
}

void graphics_context_reset(graphics_context_t* context, char* static_segment) {
    context->tlistp = &context->tlist;
    context->glistp = &context->view.dynamic.glist[0];

    gSPSegment(context->glistp++, 0, 0);
    gSPSegment(context->glistp++, STATIC_SEGMENT, osVirtualToPhysical(static_segment));
    gSPSegment(context->glistp++, RSP_FB_SEGMENT, osVirtualToPhysical(context->framebuffer[context->current_fb]));

    gSPDisplayList(context->glistp++, context->rdp_init);
    gSPDisplayList(context->glistp++, context->rsp_init);
    gSPDisplayList(context->glistp++, context->clear_fb);
}

void graphics_context_end(graphics_context_t* context) {
    gDPFullSync(context->glistp++);
    gSPEndDisplayList(context->glistp++);
}

void graphics_context_setup_tlist(graphics_context_t* context) {
    context->tlistp->t.ucode_boot = (uint64_t*)rspbootTextStart;
    context->tlistp->t.ucode_boot_size = (uint32_t)rspbootTextEnd - (uint32_t)rspbootTextStart;

    if(context->ucode) {
        context->tlistp->t.ucode = (uint64_t*)gspF3DEX2_fifoTextStart;
        context->tlistp->t.ucode_data = (uint64_t*)gspF3DEX2_fifoDataStart;
        context->tlistp->t.output_buff_size = (uint64_t*)((int)rdp_output + (int)(RDP_OUTPUT_LEN * sizeof(uint64_t)));
    } else {
        context->tlistp->t.ucode = (uint64_t*)gspF3DEX2_xbusTextStart;
        context->tlistp->t.ucode_data = (uint64_t*)gspF3DEX2_xbusDataStart;
    }

    context->tlistp->t.data_ptr = (uint64_t*)context->view.dynamic.glist;
    context->tlistp->t.data_size = (uint32_t)((context->glistp - context->view.dynamic.glist) * sizeof(Gfx));
}

void graphics_context_swapfb(graphics_context_t* context) {
    osViSwapBuffer(context->framebuffer[context->current_fb]);
    context->current_fb = 1 - context->current_fb;
}


void guPositionLookF(mtxf_t mf, camera_t* camera)
{
    vec3f_t backwd;

    guMtxIdentF(mf);

    backwd = vec3f_multiplyf(camera->forward, -1.0f);

    mf[0][0] = camera->right.x;
	mf[1][0] = camera->right.y;
	mf[2][0] = camera->right.z;

	mf[0][1] = camera->up.x;
	mf[1][1] = camera->up.y;
	mf[2][1] = camera->up.z;

	mf[0][2] = backwd.x;
	mf[1][2] = backwd.y;
	mf[2][2] = backwd.z;

    mf[3][0] = -vec3f_dot(&camera->right, &camera->actor.position);
    mf[3][1] = -vec3f_dot(&camera->up, &camera->actor.position);
    mf[3][2] = -vec3f_dot(&backwd, &camera->actor.position);
    mf[0][3] = 0;
	mf[1][3] = 0;
	mf[2][3] = 0;
    mf[3][3] = 1.0f;
}

void guPositionLook(Mtx* m, camera_t* camera) {
    mtxf_t mf;
    guPositionLookF(mf, camera);
    guMtxF2L(mf, m);
}

#endif /* GRAPHICS_H */

