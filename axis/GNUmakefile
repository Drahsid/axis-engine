include $(ROOT)/usr/include/make/PRdefs

PRDEPTH = ../..

FINAL = NO
ifeq ($(FINAL), YES)
OPTIMIZER       = -O1 -std=gnu11
LCDEFS          = -DNDEBUG -D_FINALROM -DF3DEX_GBI_2
N64LIB          = -lultra_rom
else
OPTIMIZER       = -g -std=gnu11
LCDEFS          = -DDEBUG -DF3DEX_GBI_2
N64LIB          = -lultra_d
endif

APP =		main.out

TARGETS =	main.n64

TEXHFILES =

SPRITES =

HFILES =	main.h inttypes.h video.h printf.h graphics.h

CODEFILES   =	main.c dram_stack.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c rsp_framebuffer.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS =	-I.
LCOPTS =	-mno-shared -G 0
LDFLAGS =	$(MKDEPOPT)  -L$(ROOT)/usr/lib $(N64LIB) -L$(N64_LIBGCCDIR) -lgcc

LDIRT  =	$(APP) *.so

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

ifeq ($(FINAL), YES)
$(TARGETS) $(APP):      spec $(OBJECTS)
	$(MAKEROM) -s 9 -r $(TARGETS) -e $(APP) spec
	makemask $(TARGETS)
else
$(TARGETS) $(APP):      spec $(OBJECTS)
	$(MAKEROM) -s 9 -r $(TARGETS) -e $(APP) spec
	makemask $(TARGETS)
endif