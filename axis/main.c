#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>

#include "main.h"
#include "printf.h"
#include "video.h"
#include "stdint.h"
#include "heap.h"
#include "filesystem.h"
#include "graphics.h"
#include "input.h"

extern char _codeSegmentEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];
extern char _filesystemSegmentRomStart[], _filesystemSegmentRomEnd[];

uint64_t g_boot_stack[BOOTSTACKSIZE_NUM];

void idleproc(void*);
void mainproc(void*);
void drawproc(void*);
void contproc(void*);


OSThread g_threads[THREAD_COUNT];
uint64_t g_stacks[THREAD_COUNT][STACKSIZE_NUM];
uint32_t g_memory_size;

filesystem_info_t g_filesystem;
graphics_context_t g_graphics_context;
input_context_t g_input;

// TODO: figure out what to do with this
#define NUM_PI_MSGS (8)
static OSMesgQueue g_pi_message_queue;
static OSMesg g_pi_messages[NUM_PI_MSGS];


void boot(void* arg)
{
    osInitialize();

	g_memory_size = osGetMemSize();

	printf("booted with %X memory\n", g_memory_size);
    osCreateThread(&g_threads[THREAD_IDLE], THREAD_IDLE + 1, idleproc, arg, g_stacks[THREAD_IDLE] + STACKSIZE_NUM, 10);
    osStartThread(&g_threads[THREAD_IDLE]);
}

void idleproc(void* arg)
{
	heap_construct(0x80800000u - (g_memory_size / 2), g_memory_size / 2);
	printf("heap construct\n");

	osCreatePiManager(OS_PRIORITY_PIMGR, &g_pi_message_queue, g_pi_messages, NUM_PI_MSGS);

	osCreateThread(&g_threads[THREAD_MAIN], THREAD_MAIN + 1, mainproc, arg, ((uint8_t*)&g_stacks[THREAD_MAIN][0]) + STACKSIZE, 11);
	osCreateThread(&g_threads[THREAD_DRAW], THREAD_DRAW + 1, drawproc, arg, ((uint8_t*)&g_stacks[THREAD_DRAW][0]) + STACKSIZE, 12);
	osCreateThread(&g_threads[THREAD_CONT], THREAD_CONT + 1, contproc, arg, ((uint8_t*)&g_stacks[THREAD_CONT][0]) + STACKSIZE, 10);

	osStartThread(&g_threads[THREAD_MAIN]);
	osStartThread(&g_threads[THREAD_DRAW]);
	osStartThread(&g_threads[THREAD_CONT]);

	osSetThreadPri(0, 0);
	for (;;);
}


void mainproc(void* arg)
{
	OSTime last_time;
	OSTime this_time;

	printf("mainproc go vroom\n");

	graphics_context_construct(&g_graphics_context);
	printf("gfx constructed\n");

	filesystem_info_construct(&g_filesystem, osCartRomInit(), _codeSegmentEnd, _filesystemSegmentRomStart);
	printf("filesystem constructed\n");

	char* sample_text = filesystem_info_alloc_and_read_file(&g_filesystem, "/data/storytime/story.txt");
	printf("%s\n\n", sample_text);

	this_time = osGetTime();
	last_time = osGetTime();

	for(;;) {
		this_time = osGetTime();
		g_graphics_context.rsp_ticks = OS_CYCLES_TO_USEC(this_time - last_time);

		// TODO: if there is a way to do this on demand instead if in a separate thread, I want to find it
		if (g_input.controller_valid && !g_input.controller_read) {
			osContStartReadData(&g_graphics_context.rsp_message_queue);
			g_input.controller_read = 1;
		}

		last_time = this_time;
		osRecvMesg(&g_graphics_context.rsp_message_queue, NULL, OS_MESG_BLOCK);
	}
}

void drawproc(void* arg) {
	OSTime last_time;
	OSTime this_time;

	float pitch = 0.0f;
	float yaw = 1.5707f;
	float theta = 0.0f;

	// I wrote this like this to motivate poe to finish and upload his vector superior header
	// Hope you wince while reading this
	float px = 0.0f;
	float py = 0.0f;
	float pz = -100.0f;

	float vx = 0.0f;
	float vy = 0.0f;
	float vz = 0.0f;

	float fx = 0.0f;
	float fy = 0.0f;
	float fz = 0.0f;

	float rx = 0.0f;
	float ry = 0.0f;
	float rz = 0.0f;

	float ux = 0.0f;
	float uy = 1.0f;
	float uz = 0.0f;

	float ex = 0.0f;
	float ey = 0.0f;
	float ez = 0.0f;

	int v = 0;
	int h = 0;
	int j = 0;

	printf("drawproc go vroom\n");

	this_time = osGetTime();
	last_time = osGetTime();

	for (;;) {
		v = 0;
		h = 0;
		j = 0;
		theta += 0.01f;

		this_time = osGetTime();
		g_graphics_context.rdp_ticks = OS_CYCLES_TO_USEC(this_time - last_time);

		Gfx shadetri_dl[] = {
			gsSPMatrix(osVirtualToPhysical(&(g_graphics_context.view.dynamic.projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH),
			gsSPMatrix(osVirtualToPhysical(&(g_graphics_context.view.dynamic.viewing)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH),
			gsDPPipeSync(),
			gsDPSetCycleType(G_CYC_1CYCLE),
			gsDPSetRenderMode(G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2),
			gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
			gsSPVertex(&(shade_vtx[0]), 4, 0),
			gsSP1Triangle(0, 1, 2, 0),
			gsSP1Triangle(0, 2, 3, 0),
			gsSPEndDisplayList()
		};


		// this stuff should really be in the main loop lol
		{
			pitch += g_input.controller[0].stick_now.y * 0.0725f;
			yaw += g_input.controller[0].stick_now.x * 0.0725f;

			// constrain and preserve
			if (pitch > 89.0f * (3.14169f / 180.0f)) pitch = 89.0f * (3.14169f / 180.0f);
			if (pitch < -89.0f * (3.14169f / 180.0f)) pitch = -89.0f * (3.14169f / 180.0f);
			if (yaw > (3.14159f * 2)) yaw -= (3.14159f * 2) * 2;
			if (yaw < -(3.14159f * 2)) yaw += (3.14159f * 2) * 2;

			if (g_input.controller[0].buttons[BUTTON_INDEX_CUP].state >= BUTTON_STATE_PRESSED) v += 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_CDOWN].state >= BUTTON_STATE_PRESSED) v -= 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_CRIGHT].state >= BUTTON_STATE_PRESSED) h -= 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_CLEFT].state >= BUTTON_STATE_PRESSED) h += 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_RT].state >= BUTTON_STATE_PRESSED) j += 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_LT].state >= BUTTON_STATE_PRESSED) j -= 1;

			// calculate the euler orbit position based on pitch & yaw
			ex = cosf(pitch) * cosf(yaw);
			ey = -sinf(pitch);
			ez = cosf(pitch) * sinf(yaw);

			// which just so happens to also be our forward vector
			fx = ex;
			fy = ey;
			fz = ez;

			// cross it with whatever is the last up vector
			rx = fy * uz - uy * fz;
			ry = fz * ux - uz * fx;
			rz = fx * uy - ux * fy;

			// normalize
			ux = sqrtf(fx * fx + fy * fy + fz * fz);
			fx /= ux;
			fy /= ux;
			fz /= ux;

			ux = sqrtf(rx * rx + ry * ry + rz * rz);
			rx /= ux;
			ry /= ux;
			rz /= ux;

			// convert to desired dir and check magnitude then normalize
			vx = (fx * v) + (rx * h);
			vy = (fy * v) + (ry * h);
			vz = (fz * v) + (rz * h);

			ux = sqrtf(vx * vx + vy * vy + vz * vz);
			if (ux > 0) {
				vx /= ux;
				vy /= ux;
				vz /= ux;
			}

			// same for up and down
			if (j != 0) {
				ux = ry * fz - fy * rz;
				uy = rz * fx - fz * rx;
				uz = rx * fy - fx * ry;

				rx = sqrtf(ux * ux + uy * uy + uz * uz);
				if (rx != 0) {
					ux /= rx;
					uy /= rx;
					uz /= rx;

					px += ux * (j * 5.0f);
					py += uy * (j * 5.0f);
					pz += uz * (j * 5.0f);
				}
			}

			// move
			px += vx * 5.0f;
			py += vy * 5.0f;
			pz += vz * 5.0f;

			// lmao, true to the concept, 'onetri-3d;' rotates the perspective just like onetri rotates the ortho
			ux = cosf(theta);
			uy = -sinf(theta);
			uz = sinf(theta);

			rx = sqrtf(ux * ux + uy * uy + uz * uz);
			ux /= rx;
			uy /= rx;
			uz /= rx;
		}

		guPerspective(&g_graphics_context.view.dynamic.projection, &g_graphics_context.view.persp_norm, g_graphics_context.view.fov, g_graphics_context.view.aspect, g_graphics_context.view.near, g_graphics_context.view.far, g_graphics_context.view.scale);
		guLookAt(&g_graphics_context.view.dynamic.viewing,
			px, py, pz, // eye
			px + ex, py + ey, pz + ez, // at / focus
			ux, uy, uz // up
			/*0, 1.0f, 0*/
			);

		gSPPerspNormalize(g_graphics_context.glistp++, g_graphics_context.view.persp_norm);

		graphics_context_reset(&g_graphics_context, _codeSegmentEnd);

		// do work
		gSPDisplayList(g_graphics_context.glistp++, shadetri_dl);

		graphics_context_end(&g_graphics_context);

		// set up task list
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


		// perform tasks
		osWritebackDCache(&g_graphics_context.view.dynamic, sizeof(dynamic_t));
		osSpTaskStart(g_graphics_context.tlistp);

		last_time = this_time;

		// wait for rdp to finish
		osRecvMesg(&g_graphics_context.rdp_message_queue, NULL, OS_MESG_BLOCK);

		graphics_context_swapfb(&g_graphics_context);

		// wait for vi to finish
		osRecvMesg(&g_graphics_context.vsync_message_queue, NULL, OS_MESG_BLOCK);
	}
}

void contproc(void* arg) {
	printf("contproc go vroom\n");

	input_context_construct(&g_input);
	printf("input constructed\n");

	for(;;) {
		input_context_step(&g_input);

		// you would probably interpret input in the mainloop since this loop is dedicated to polling input, this is just for testing
		{
			if (g_input.controller[0].buttons[BUTTON_INDEX_A].state == BUTTON_STATE_PRESSED) {
				printf("A button pressed!\n");
			}

			if (g_input.controller[0].buttons[BUTTON_INDEX_A].state == BUTTON_STATE_DOWN) {
				printf("A button is down!\n");
			}

			if (g_input.controller[0].buttons[BUTTON_INDEX_A].state == BUTTON_STATE_RELEASED) {
				printf("A button released!\n");
			}

			if (g_input.controller[0].stick_now.x != g_input.controller[0].stick_before.x) {
				// being able to printf so fast lags emulator if you start treating the stick like mario party lol
				//printf("Stick moved horizontally! new %f - old %f = %f\n", g_input.controller[0].stick_now.x, g_input.controller[0].stick_before.x, g_input.controller[0].stick_now.x - g_input.controller[0].stick_before.x);
			}
		}

		// wait for update
		osRecvMesg(&g_input.controller_message_queue, NULL, OS_MESG_BLOCK);
	}
}

