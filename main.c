#include <ultra64.h>
#include "inttypes.h"
#include "main.h"
#include "video.h"
#include "graphics.h"
#include "matrix.h"
#include "vectortypes.h"
#include "printf.h"

#define RELEASE_N64

/*
 * Symbol genererated by "makerom" to indicate the end of the code segment
 * in virtual (and physical) memory
 */
extern char _codeSegmentEnd[];

/*
 * Symbols generated by "makerom" to tell us where the static segment is
 * in ROM.
 */
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];

// We don't have to manually create the stack when on PC
#ifdef RELEASE_N64

uint64_t boot_stack[STACKSIZE / sizeof(uint64_t)];
uint64_t idle_thread_stack[STACKSIZENUMIDLE];
uint64_t game_thread_stack[STACKSIZENUM];
uint64_t dram_stack[SP_DRAM_STACK_SIZE64];
uint64_t rdp_output[RDP_OUTPUT_LEN / 8];

OSThread idle_thread;
OSThread game_thread;

OSTask g_tlist;

OSPiHandle* g_rom_handle;
#endif

uint64_t g_os_mem_size;
graphics_context_t g_gfx_context;

static void idleproc(void*);
static void gameproc(void*);

void boot(void* args) {
    osInitialize();
    printf("OS Init\n");

    g_os_mem_size = osGetMemSize();
    g_rom_handle = osCartRomInit();
    printf("Mem size: 0x%X, rom handle: 0x%X", g_os_mem_size, g_rom_handle);

    osCreateThread(&idle_thread, 1, idleproc, args, &idle_thread_stack[STACKSIZENUMIDLE], 12);
    osStartThread(&idle_thread);

    printf("Boot done.\n");
}

void idleproc(void* args) {
    printf("idle starting\n");
    osCreateThread(&game_thread, 3, gameproc, args, &game_thread_stack[STACKSIZENUM], 12);
    osStartThread(&game_thread);
    osSetThreadPri(0, 0);
    printf("Idle started\n");
    for(;;);
}


void gameproc(void* args) {
    osCreateViManager(OS_PRIORITY_VIMGR);
    osViSetMode(&osViModeTable[/*VI_MODE_NTSC_HPF2*/ VI_MODE_NTSC_LPF1]);

    printf("vi init\n");

    graphics_context_t* gfx_context = &g_gfx_context;

    OSTask* tlistp = &g_tlist;
    Gfx* glistp = &gfx_context->view.dyn.glist;

    graphics_context_t_construct(gfx_context);
    printf("gfx constructed\n");

    osCreateMesgQueue(&gfx_context->rdp_message_queue, &gfx_context->rdp_message_buffer, 1);
    osSetEventMesg(OS_EVENT_DP, &gfx_context->rdp_message_queue, NULL);

    osCreateMesgQueue(&gfx_context->sync_message_queue, &gfx_context->sync_message_buffer, 1);
    osViSetEvent(&gfx_context->sync_message_queue, NULL, 1);

    printf("created message queues\n");

    Gfx rspinit[5] = {
        gsSPViewport(&gfx_context->viewport),
        gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH
                | G_FOG | G_LIGHTING | G_TEXTURE_GEN
                | G_TEXTURE_GEN_LINEAR | G_LOD),
        gsSPTexture(0, 0, 0, 0, G_OFF),
        gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList()
    };

    Gfx rdpinit[16] = {
        gsDPSetCycleType(G_CYC_1CYCLE),
        gsDPPipelineMode(G_PM_1PRIMITIVE),
        gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT),
        gsDPSetTextureLOD(G_TL_TILE),
        gsDPSetTextureLUT(G_TT_NONE),
        gsDPSetTextureDetail(G_TD_CLAMP),
        gsDPSetTexturePersp(G_TP_PERSP),
        gsDPSetTextureFilter(G_TF_BILERP),
        gsDPSetTextureConvert(G_TC_FILT),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetCombineKey(G_CK_NONE),
        gsDPSetAlphaCompare(G_AC_NONE),
        gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
        gsDPSetColorDither(G_CD_DISABLE),
        gsDPPipeSync(),
        gsSPEndDisplayList()
    };

    Gfx clearcfb[8] = {
        gsDPSetCycleType(G_CYC_FILL),
        gsDPSetColorImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, gfx_context->rsp_framebuffer),
        gsDPSetFillColor(GPACK_RGBA5551(64,64,64,1) << 16 | GPACK_RGBA5551(64,64,64,1)),
        gsDPFillRectangle(0, 0, SCREEN_WD-1, SCREEN_HT-1),
        gsDPPipeSync(),
        gsDPSetFillColor(GPACK_RGBA5551(64,64,255,1) << 16 | GPACK_RGBA5551(64,64,255,1)),
        gsDPFillRectangle(20, 20, SCREEN_WD-20, SCREEN_HT-20),
        gsSPEndDisplayList()
    };


    *(int*)(0x80700000) = 0;

    for(;;) {
        int* frames = (int*)(0x80700000);
        *frames += 1;
        printf("frame %d", *frames);

        glistp = gfx_context->view.dyn.glist;
        tlistp = &g_tlist;

        printf("glistp and glistp reset: %X, %X\n", glistp, tlistp);

        gSPSegment(glistp++, PHYSICAL_SEGMENT, 0);
        gSPSegment(glistp++, STATIC_SEGMENT, osVirtualToPhysical(_codeSegmentEnd));
        gSPSegment(glistp++, DYNAMIC_SEGMENT, osVirtualToPhysical(gfx_context->framebuffer[gfx_context->current_fb]));

        printf("segmetns\n");

        guPerspective(&gfx_context->view.dyn.projection, &gfx_context->view.persp_norm, /*1.79768912955f*/ 45.0f, SCREEN_WD / SCREEN_HT, 0.01f, 1000.0f, 1.0f);
        guRotate(&gfx_context->view.dyn.modeling, 0.0f, 0.0f, 0.0f, 1.0f);

        printf("persp_rot");

        gSPDisplayList(glistp++, rdpinit);
        gSPDisplayList(glistp++, rspinit);
        gSPDisplayList(glistp++, clearcfb);

        printf("gSPDisplayLists\n");

        gDPFullSync(glistp++);
	    gSPEndDisplayList(glistp++);

        tlistp->t.type = M_GFXTASK;
        tlistp->t.flags = 0;

        tlistp->t.ucode_boot = (uint64_t*)rspbootTextStart;
        tlistp->t.ucode_boot_size = (uint32_t)rspbootTextEnd - (uint32_t)rspbootTextStart;

        tlistp->t.ucode = (uint64_t*)gspF3DEX2_fifoTextStart;
        tlistp->t.ucode_data = (uint64_t*)gspF3DEX2_fifoDataStart;
        tlistp->t.ucode_size = SP_UCODE_SIZE;
        tlistp->t.ucode_data_size = SP_UCODE_DATA_SIZE;

        tlistp->t.dram_stack = &dram_stack[0];
        tlistp->t.dram_stack_size = SP_DRAM_STACK_SIZE8;

        //tlistp->t.output_buff = &rdp_output[0];
        //tlistp->t.output_buff_size = (uint64_t*)&rdp_output[0] + (RDP_OUTPUT_LEN * sizeof(uint64_t));
        tlistp->t.output_buff = (uint64_t*)0;
		tlistp->t.output_buff_size = (uint64_t*)0;

        tlistp->t.data_ptr = (uint64_t*)gfx_context->view.dyn.glist;
        tlistp->t.data_size = (uint32_t)((glistp - gfx_context->view.dyn.glist) * sizeof(Gfx));

        printf("set up tlistp\n");

        //osWritebackDCache(&gfx_context->view.dyn, sizeof(dynamic_t));
        osWritebackDCacheAll();
        printf("osWritebackDCache\n");

        osSpTaskLoad(tlistp);
        osSpTaskStart(tlistp);

        printf("taskStart\n");

        osRecvMesg(&gfx_context->sync_message_queue, NULL, OS_MESG_BLOCK);
        osRecvMesg(&gfx_context->rdp_message_queue, NULL, OS_MESG_BLOCK);

        printf("waited!\n");

        if (MQ_IS_FULL(&gfx_context->sync_message_queue)) osRecvMesg(&gfx_context->sync_message_queue, NULL, OS_MESG_BLOCK);

        osViSwapBuffer(gfx_context->framebuffer[gfx_context->current_fb]);

        printf("swapped buffer\n");

        gfx_context->current_fb = 1 - gfx_context->current_fb;

        printf("fb changed\n");
    }
}



