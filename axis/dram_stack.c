#include <ultra64.h>
#include "video.h"
#include "stdint.h"

uint64_t dram_stack[SP_DRAM_STACK_SIZE64] __attribute__((aligned (16))); /* used for matrix stack */
uint64_t rdp_output[RDP_OUTPUT_LEN] __attribute__((aligned (16))); /* buffer for RDP DL */

