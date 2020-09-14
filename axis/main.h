#ifndef MAIN_H
#define MAIN_H

#include "video.h"
#include "stdint.h"

#define	STATIC_SEGMENT	1
#define	RSP_FB_SEGMENT	2

#define	STACKSIZE	0x2000
#define STACKSIZE_NUM (STACKSIZE / sizeof(unsigned long long))

#ifdef _LANGUAGE_C

extern COLOR_DEPTH_TYPE g_rsp_framebuffer[];

extern Vtx shade_vtx[];

extern uint64_t dram_stack[];
extern uint64_t rdp_output[];

#endif	/* _LANGUAGE_C */

#endif

