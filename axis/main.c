#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>

#include "main.h"
#include "heap.h"
#include "printf.h"
#include "video.h"
#include "stdint.h"
#include "filesystem.h"
#include "graphics.h"

extern char _codeSegmentEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];

uint64_t g_boot_stack[STACKSIZE_NUM];

static void	idleproc(void*);
static void	mainproc(void*);

static OSThread	idle_thread;
static uint64_t	idle_thread_stack[STACKSIZE_NUM];

static OSThread	main_thread;
static uint64_t	main_thread_stack[STACKSIZE_NUM];

#define NUM_PI_MSGS (8)

static OSMesg pi_messages[NUM_PI_MSGS];
static OSMesgQueue pi_message_queue;

OSMesgQueue rsp_message_queue;
OSMesg rsp_message_buffer;

filesystem_info_t g_filesystem;
graphics_context_t g_graphics_context;
OSPiHandle* g_handler;
uint32_t g_memory_size;

float g_theta = 0.0f;

void boot(void* arg)
{
    osInitialize();

	g_memory_size = osGetMemSize();
    g_handler = osCartRomInit();

	printf("booted with %X memory\n", g_memory_size);
    osCreateThread(&idle_thread, 1, idleproc, 0, idle_thread_stack + STACKSIZE_NUM, 10);
    osStartThread(&idle_thread);
}

void idleproc(void* arg)
{
	heap_construct(0x80800000u - (g_memory_size / 4), g_memory_size / 4);
	printf("heap construct\n");

	void** test[8];
	test[0] = malloc(16);
	test[1] = malloc(32);
	test[2] = malloc(64);
	test[3] = malloc(256);
	test[4] = malloc(1024);
	test[5] = malloc(2048);
	test[6] = malloc(4096);
	test[7] = malloc(8192);

	printf("malloc test: ");

	for (int i = 0; i < 8; i++) {
		printf("%X, ", test[i]);
		if (i % 2 == 1) {
			free(test[i]);
			test[i] = malloc(i * 512);
		}
	}

	printf("\nevery other alloc'd data freed\n");

	printf("malloc test: ");

	for (int i = 0; i < 8; i++) {
		printf("%X, ", test[i]);
		if (i % 2 == 1) free(test[i]);
	}

    osCreateViManager(OS_PRIORITY_VIMGR);
    osViSetMode(&osViModeTable[OS_VI_NTSC_HPF1]);

    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);

    osCreatePiManager(OS_PRIORITY_PIMGR, &pi_message_queue, pi_messages, NUM_PI_MSGS);

    osCreateThread(&main_thread, 3, mainproc, arg, main_thread_stack + STACKSIZE_NUM, 10);
    osStartThread(&main_thread);

    osSetThreadPri(0, 0);

	printf("idle proc sleeping\n");

    for (;;);
}

void mainproc(void* arg)
{
	printf("mainproc go vroom\n");
    char* static_segment;
    OSTime start_time;

	graphics_context_construct(&g_graphics_context);
	printf("gfx constructed\n");

    osCreateMesgQueue(&rsp_message_queue, &rsp_message_buffer, 1);
    osSetEventMesg(OS_EVENT_SP, &rsp_message_queue, NULL);

    osCreateMesgQueue(&g_graphics_context.rdp_message_queue, &g_graphics_context.rdp_message_buffer, 1);
    osSetEventMesg(OS_EVENT_DP, &g_graphics_context.rdp_message_queue, NULL);

    osCreateMesgQueue(&g_graphics_context.vsync_message_queue, &g_graphics_context.vsync_message_buffer, 1);
    osViSetEvent(&g_graphics_context.vsync_message_queue, NULL, 1);

	printf("events setup\n");

    static_segment = _codeSegmentEnd;

	filesystem_info_construct(&g_filesystem, g_handler, static_segment);
	printf("filesystem setup\n");

	char* sample_text = 0x80400000u;
	filesystem_info_read_file("/data/storytime/story.txt", sample_text, &g_filesystem);
	printf("\n%s\n", sample_text);

	int frame = 0;
    for(;;) {

		frame++;

		Gfx shadetri_dl[] = {
			gsSPMatrix(osVirtualToPhysical(&(g_graphics_context.view.dynamic.projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH),
			gsSPMatrix(osVirtualToPhysical(&(g_graphics_context.view.dynamic.modeling)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH),
			gsDPPipeSync(),
			gsDPSetCycleType(G_CYC_1CYCLE),
			gsDPSetRenderMode(G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2),
			gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
			gsSPVertex(&(shade_vtx[0]), 4, 0),
			gsSP1Triangle(0, 1, 2, 0),
			gsSP1Triangle(0, 2, 3, 0),
			gsSPEndDisplayList()
		};

		guOrtho(&g_graphics_context.view.dynamic.projection,
			-(float)SCREEN_WD / 2.0f, (float)SCREEN_WD / 2.0f,
			-(float)SCREEN_HT / 2.0f, (float)SCREEN_HT / 2.0f,
			1.0f, 10.0f, 1.0f);
		//guPerspective(&g_graphics_context.view.dynamic.projection, &g_graphics_context.view.persp_norm, g_graphics_context.view.fov, g_graphics_context.view.aspect, g_graphics_context.view.near, g_graphics_context.view.far, g_graphics_context.view.scale);
		guRotate(&g_graphics_context.view.dynamic.modeling, g_theta, 0.0f, 0.0f, 1.0f);

		graphics_context_reset(&g_graphics_context, static_segment);

		// do work
		gSPDisplayList(g_graphics_context.glistp++, shadetri_dl);

		graphics_context_end(&g_graphics_context);

		g_graphics_context.tlistp->t.ucode_boot = (uint64_t*)rspbootTextStart;
		g_graphics_context.tlistp->t.ucode_boot_size = (uint32_t)rspbootTextEnd - (uint32_t)rspbootTextStart;


		if(g_graphics_context.ucode) {
			g_graphics_context.tlistp->t.ucode = (uint64_t*)gspF3DEX2_fifoTextStart;
			g_graphics_context.tlistp->t.ucode_data = (uint64_t*)gspF3DEX2_fifoDataStart;
			g_graphics_context.tlistp->t.output_buff_size = (uint64_t*)((int)rdp_output + (int)(RDP_OUTPUT_LEN * sizeof(uint64_t)));
		} else {
			g_graphics_context.tlistp->t.ucode = (uint64_t*)gspF3DEX2_xbusTextStart;
			g_graphics_context.tlistp->t.ucode_data = (uint64_t*)gspF3DEX2_xbusDataStart;
		}

		g_graphics_context.tlistp->t.data_ptr = (uint64_t*)g_graphics_context.view.dynamic.glist;
		g_graphics_context.tlistp->t.data_size = (uint32_t)((g_graphics_context.glistp - g_graphics_context.view.dynamic.glist) * sizeof(Gfx));

		osWritebackDCache(&g_graphics_context.view.dynamic, sizeof(dynamic_t));

		start_time = osGetTime();

		osSpTaskStart(g_graphics_context.tlistp);
		osRecvMesg(&rsp_message_queue, NULL, OS_MESG_BLOCK);

		g_graphics_context.rsp_ticks = osGetTime() - start_time;

		osRecvMesg(&g_graphics_context.rdp_message_queue, NULL, OS_MESG_BLOCK);

		g_graphics_context.rdp_ticks = osGetTime() - start_time ;

		graphics_context_swapfb(&g_graphics_context);

		if (MQ_IS_FULL(&g_graphics_context.vsync_message_queue)) osRecvMesg(&g_graphics_context.vsync_message_queue, NULL, OS_MESG_BLOCK);
		osRecvMesg(&g_graphics_context.vsync_message_queue, NULL, OS_MESG_BLOCK);

		g_theta += 1.0F;
		//printf("FRAME: %d\n", frame);
    }
}
