include $(ROOT)/usr/include/make/PRdefs

PRDEPTH = ../..

FINAL = YES
ifeq ($(FINAL), YES)
OPTIMIZE	=	-O1
LCDEFS		=	-DNDEBUG -D_FINALROM -DF3DEX_GBI_2
N64LIB		=	-lultra_rom
ROMSIZE		=	24
else
OPTIMIZER	=	-g
LCDEFS		=	-DDEBUG -DF3DEX_GBI_2
N64LIB		=	-lultra_d
ROMSIZE		=	24
endif

APP			=	main.out
TARGETS		=	main.n64
FILESYSTEM	=	filesystem.hfs
FSPACK		=	../tools/fspack/fspack

CODEFILES   =	main.c engine/segment/dram_stack.c ../include/math/pcg/pcg_basic.c
DATAFILES   =	engine/segment/static.c engine/segment/rsp_framebuffer.c

DATAOBJECTS =	$(DATAFILES:.c=.o)
CODEOBJECTS =	$(CODEFILES:.c=.o)
CODESEGMENT =	codesegment.o
OBJECTS		=	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS		=	-I. -I../include
LCOPTS		=	-G 0 -march=vr4300 -mno-check-zero-division -mdivide-breaks -mno-shared -std=gnu11
LDFLAGS		=	$(MKDEPOPT) -L$(ROOT)/usr/lib $(N64LIB) -L$(N64_LIBGCCDIR) -lgcc
LDIRT 		=	$(APP) *.so

default:	$(TARGETS)
include $(COMMONRULES)


$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

#

ifeq ($(FINAL), YES)
$(TARGETS) $(APP):      spec $(OBJECTS)
	../tools/fspack/fspack -d ./filesystem -o filesystem.hfs -b
	$(MAKEROM) -s $(ROMSIZE) -r $(TARGETS) -e $(APP) spec
	makemask $(TARGETS)
else
$(TARGETS) $(APP):      spec $(OBJECTS)
	../tools/fspack/fspack -d ./filesystem -o filesystem.hfs -b
	$(MAKEROM) -s $(ROMSIZE) -r $(TARGETS) -e $(APP) spec
	makemask $(TARGETS)
endif