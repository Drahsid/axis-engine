#include "main.h"

beginseg
	name "code"
	flags BOOT OBJECT
	entry boot
	stack boot_stack + STACKSIZE
	include "codesegment.o"
	include "$(ROOT)/usr/lib/PR/rspboot.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.xbus.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.fifo.o"
endseg

beginseg
	name "static"
	flags OBJECT
	number STATIC_SEGMENT
endseg

beginseg
	name "dynamic"
	flags OBJECT
	number DYNAMIC_SEGMENT
endseg

beginwave
    name "main"
    include "code"
	include "static"
	include "dynamic"
endwave

