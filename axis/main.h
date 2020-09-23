#ifndef MAIN_H
#define MAIN_H

#include "video.h"
#include "stdint.h"

#define	STATIC_SEGMENT 1
#define	RSP_FB_SEGMENT 2
#define FILESYSTEM_SEGMENT 3

#define BOOTSTACKSIZE 0x800
#define BOOTSTACKSIZE_NUM (BOOTSTACKSIZE / sizeof(uint64_t))

#define	STACKSIZE 0x2000
#define STACKSIZE_NUM (STACKSIZE / sizeof(uint64_t))

#ifdef _LANGUAGE_C

enum thread_id {
	THREAD_IDLE,
	THREAD_MAIN,
	THREAD_DRAW,
	THREAD_CONT,
	THREAD_COUNT
};

extern COLOR_DEPTH_TYPE g_rsp_framebuffer[];

extern Vtx shade_vtx[];

extern uint64_t dram_stack[];
extern uint64_t rdp_output[];

#endif	/* _LANGUAGE_C */

#endif /* MAIN_H */

