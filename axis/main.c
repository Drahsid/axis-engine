#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>

#include "engine/main.h"

#define EXAMPLE_SNAKE

#ifdef EXAMPLE_ONETRI
#include "examples/onetri/app_context.h"
#include "examples/onetri/app_main.h"
#include "examples/onetri/app_draw.h"
#include "examples/onetri/app_cont.h"
#elif defined EXAMPLE_SNAKE
#include "examples/snake/app_context.h"
#include "examples/snake/app_main.h"
#include "examples/snake/app_draw.h"
#include "examples/snake/app_cont.h"
#endif

#include "math/math_common.h"
#include "engine/printf.h"
#include "stdint.h"

extern char _codeSegmentEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];
extern char _filesystemSegmentRomStart[], _filesystemSegmentRomEnd[];

uint64_t g_boot_stack[BOOTSTACKSIZE_NUM];

void boot(void*);
void idleproc(void*);
void mainproc(void*);
void drawproc(void*);
void contproc(void*);

// this structure stores global information for the app that is accessible by everything
app_context_t g_context;

void boot(void* arg)
{
    osInitialize();

	// this function sets up the threads and basic info such as memory size, and the main heap
	app_context_construct(&g_context, g_boot_stack, idleproc, mainproc, drawproc, contproc, arg);

    osStartThread(&g_context.threads[THREAD_IDLE]);
}

void idleproc(void* arg)
{
	/* this is a basic example for how one could manage a modular game state.
	 * You might also want to consider alternatively compiling the functions into their own objects and loading them if you want more control
	 * Here, I just have them in their own headers
	 */
	g_context.main_step = mainstep_default;
	g_context.draw_step = drawstep_default;
	g_context.cont_step = contstep_default;

	osCreatePiManager(OS_PRIORITY_PIMGR, &g_context.pi_message_queue, g_context.pi_messages, NUM_PI_MSGS);

	osStartThread(&g_context.threads[THREAD_MAIN]);

	osSetThreadPri(0, 0);
	for (;;);
}


void mainproc(void* arg)
{
	OSTime last_time;
	OSTime this_time;
	OSTime start_time;
	OSTime end_time;

	printf("mainproc go vroom\n");

	// this initializes the rest of the members in the app context
	app_context_initialize(&g_context);
	printf("global context initialized\n");

	// we start the controller thread late to make sure that all requisite structures are set up
	osStartThread(&g_context.threads[THREAD_CONT]);
	osStartThread(&g_context.threads[THREAD_DRAW]);

	this_time = osGetTime();
	last_time = osGetTime();
	start_time = osGetTime();
	end_time = osGetTime();

	for(;;) {
		this_time = osGetTime();
		if (g_context.main_step) g_context.main_step(&g_context, 0);

		g_context.main_time = OS_CYCLES_TO_SEC(this_time - last_time);
		last_time = this_time;

		start_time = osGetTime();
		osRecvMesg(&g_context.graphics_context.rsp_message_queue, NULL, OS_MESG_BLOCK);
		end_time = osGetTime();
		g_context.mainproc_time = OS_CYCLES_TO_SEC(end_time - start_time);
	}
}

void drawproc(void* arg) {
	OSTime last_time;
	OSTime this_time;
	OSTime start_time;
	OSTime end_time;

	printf("drawproc go vroom\n");

	this_time = osGetTime();
	last_time = osGetTime();
	start_time = osGetTime();
	end_time = osGetTime();

	for (;;) {
		this_time = osGetTime();
		if (g_context.draw_step) g_context.draw_step(&g_context, 0);
		g_context.draw_time = OS_CYCLES_TO_SEC((double)(this_time - last_time));
		last_time = this_time;

		// wait for rdp to finish
		osRecvMesg(&g_context.graphics_context.rdp_message_queue, NULL, OS_MESG_BLOCK);

		graphics_context_swapfb(&g_context.graphics_context);

		// wait for vi to finish
		start_time = osGetTime();
		osRecvMesg(&g_context.graphics_context.vsync_message_queue, NULL, OS_MESG_BLOCK);
		end_time = osGetTime();
		g_context.drawproc_time = OS_CYCLES_TO_SEC(end_time - start_time);
	}
}

void contproc(void* arg) {
	OSTime last_time;
	OSTime this_time;
	OSTime start_time;
	OSTime end_time;

	printf("contproc go vroom\n");

	this_time = osGetTime();
	last_time = osGetTime();
	start_time = osGetTime();
	end_time = osGetTime();

	for(;;) {
		this_time = osGetTime();
		if (g_context.cont_step) g_context.cont_step(&g_context, 0);
		g_context.cont_time = OS_CYCLES_TO_SEC(this_time - last_time);
		last_time = this_time;

		// wait for update
		start_time = osGetTime();
		osRecvMesg(&g_context.input_context.controller_message_queue, NULL, OS_MESG_BLOCK);
		end_time = osGetTime();
		g_context.contproc_time = OS_CYCLES_TO_SEC(end_time - start_time);
	}
}

