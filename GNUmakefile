#!smake
include $(ROOT)/usr/include/make/PRdefs

FINAL = YES
#Alternate LCDefs
#LCDEFS =	-DF3DEX_GBI -DEX
#LCDEFS =	-DF3DEX_GBI -DLX
#LCDEFS =	-DF3DEX_GBI -DLX_Rej
#LCDEFS =	-DF3DEX_GBI -DLP_Rej

ifeq ($(FINAL), YES)
OPTIMIZER       = -O2 -std=gnu99
LCDEFS			= -DNDEBUG -D_FINALROM -DF3DEX_GBI -DEX
N64LIB          = -lultra_rom

else
OPTIMIZER       = -g -std=gnu99
LCDEFS          = -DDEBUG -DF3DEX_GBI -DEX
N64LIB          = -lultra_d
endif

APP =		main.out

TARGETS =	main.n64

HFILES =	\
	inttypes.h \
	matrix.h \
	vectortypes.h \
	video.h \
	graphics.h \
	main.h

CODEFILES =	\
	main.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

CODESEGMENT =	codesegment.o

# Data files that have thier own segments:

DATAFILES =

DATAOBJECTS =	$(DATAFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS =	-I.. -I$(ROOT)/usr/include/PR
LCOPTS =	$(DFLAG) -mno-shared -G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib $(N64LIB)  -L$(N64_LIBGCCDIR) -lgcc

LDIRT  =	$(APP)

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

ifeq ($(FINAL), YES)
$(TARGETS) $(APP):      spec $(OBJECTS)
	$(MAKEROM) -s 9 -r $(TARGETS) spec
	makemask $(TARGETS)
else
$(TARGETS) $(APP):      spec $(OBJECTS)
	$(MAKEROM) -r $(TARGETS) spec
endif

