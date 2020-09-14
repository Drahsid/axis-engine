#ifndef MAIN_H
#define MAIN_H

#define	PHYSICAL_SEGMENT	0
#define	STATIC_SEGMENT		1
#define	DYNAMIC_SEGMENT		2

#define STACKSIZEBYTES  (0x8000)
#define STACKSIZEBYTESIDLE (0x5D1)
#define STACKSIZE 0x400

// Put stuff that can't be included in spec here
#ifdef _LANGUAGE_C

#define STACKSIZENUM    (STACKSIZEBYTES / sizeof(unsigned long long))
#define STACKSIZENUMIDLE (STACKSIZENUM / 16)

#define RDP_OUTPUT_LEN (4096 * 16)

#ifndef SP_DRAM_STACK_SIZE8
#define SP_DRAM_STACK_SIZE8 1
#define SP_DRAM_STACK_SIZE16 1
#define SP_DRAM_STACK_SIZE32 1
#define SP_DRAM_STACK_SIZE64 1
#endif

#ifndef SP_UCODE_SIZE
#define SP_UCODE_SIZE 1
#define SP_UCODE_DATA_SIZE 1
#endif
#endif

#endif