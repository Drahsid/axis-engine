#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>

#include "main.h"
#include "video.h"
#include "printf.h"
#include "heap.h"
#include "filesystem.h"
#include "graphics.h"
#include "input.h"
#include "stdint.h"

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
    osCreateThread(&g_threads[THREAD_IDLE], THREAD_IDLE + 1, idleproc, arg, ((uint8_t*)&g_stacks[THREAD_IDLE][0]) + STACKSIZE, 10);
    osStartThread(&g_threads[THREAD_IDLE]);
}

void idleproc(void* arg)
{
	heap_construct(0x80000000u + (g_memory_size / 2), g_memory_size / 2);
	printf("heap construct at %X with %X space\n", g_heap_start, g_heap_size);

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

	vec3f_t euler = vec_new(0, 1.5707f, 0);
	vec3f_t orbit = vec3f_zero;
	vec3f_t position = vec_new(0, 0, -100.0f);
	vec3f_t velocity = vec3f_zero;
	vec3f_t forward = vec3f_forward;
	vec3f_t right = vec3f_right;
	vec3f_t up = vec3f_up;
	vec3i_t vhj = {0, 0, 0};

	printf("drawproc go vroom\n");

	this_time = osGetTime();
	last_time = osGetTime();

	for (;;) {
		vhj.x = 0;
		vhj.y = 0;
		vhj.z = 0;

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
			euler.x += g_input.controller[0].stick_now.y * 0.0725f;
			euler.y += g_input.controller[0].stick_now.x * 0.0725f;

			// constrain and preserve
			if (euler.x > 89.0f * (3.14169f / 180.0f)) euler.x = 89.0f * (3.14169f / 180.0f);
			if (euler.x < -89.0f * (3.14169f / 180.0f)) euler.x = -89.0f * (3.14169f / 180.0f);
			if (euler.y > (3.14159f * 2)) euler.y -= (3.14159f * 2) * 2;
			if (euler.y < -(3.14159f * 2)) euler.y += (3.14159f * 2) * 2;

			if (g_input.controller[0].buttons[BUTTON_INDEX_CUP].state >= BUTTON_STATE_PRESSED) vhj.x += 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_CDOWN].state >= BUTTON_STATE_PRESSED) vhj.x -= 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_CRIGHT].state >= BUTTON_STATE_PRESSED) vhj.y -= 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_CLEFT].state >= BUTTON_STATE_PRESSED) vhj.y += 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_RT].state >= BUTTON_STATE_PRESSED) vhj.z += 1;
			if (g_input.controller[0].buttons[BUTTON_INDEX_LT].state >= BUTTON_STATE_PRESSED) vhj.z -= 1;

			// calculate the euler orbit position based on pitch & yaw
			orbit = vec_new(
				cosf(euler.x) * cosf(euler.y),
				-sinf(euler.x),
				cosf(euler.x) * sinf(euler.y));

			// which just so happens to also be our forward vector
			forward = orbit;

			// cross it with whatever is the last up vector
			right = vec3f_cross(&forward, &up);

			// normalize
			vec3f_normalize_assignment(&forward);
			vec3f_normalize_assignment(&right);

			// convert to desired dir and check magnitude then normalize
			velocity =	vec3f_add(
						vec3f_multiplyf(forward, vhj.x), vec3f_multiplyf(right, vhj.y));

			if ((up.x = vec3f_square_magnitude_p(&velocity)) > 0) {
				vec3f_dividef_assignment(&velocity, sqrtf(up.x));
			}

			// same for up and down
			if (vhj.z != 0) {
				up = vec3f_cross(&right, &forward);

				if ((right.x = vec3f_square_magnitude_p(&up)) != 0) {
					vec3f_dividef_assignment(&up, sqrtf(right.x));
					right = vec3f_multiplyf(up, vhj.z * 5.0f);
					vec3f_add_assignment(&position, &right);
				}
			}

			// move
			right = vec3f_multiplyf(velocity, 5.0f);
			vec3f_add_assignment(&position, &right);

			/* lmao, true to the concept, 'onetri-3d;' rotates the perspective just like onetri rotates the ortho
			up.x = cosf(theta);
			up.y = -sinf(theta);
			up.z = sinf(theta);

			right.x = sqrtf(up.x * up.x + up.y * up.y + up.z * up.z);
			up.x /= right.x;
			up.y /= right.x;
			up.z /= right.x;*/
			up = vec3f_up;
		}

		guPerspective(&g_graphics_context.view.dynamic.projection, &g_graphics_context.view.persp_norm, g_graphics_context.view.fov, g_graphics_context.view.aspect, g_graphics_context.view.near, g_graphics_context.view.far, g_graphics_context.view.scale);
		gSPPerspNormalize(g_graphics_context.glistp++, g_graphics_context.view.persp_norm);
		guLookAt(
			&g_graphics_context.view.dynamic.viewing,
			position.x, position.y, position.z, // eye
			position.x + orbit.x, position.y + orbit.y, position.z + orbit.z, // at / focus
			up.x, up.y, up.z // up
			/*0, 1.0f, 0*/
		);

		graphics_context_reset(&g_graphics_context, _codeSegmentEnd);

		// this would be where we actually draw stuff
		gSPDisplayList(g_graphics_context.glistp++, shadetri_dl);

		graphics_context_end(&g_graphics_context);
		graphics_context_setup_tlist(&g_graphics_context);

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

