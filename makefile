#################################################################################
#                                                                               #
# DISINTEGRATION - a 3D graphik program                                         #
# makefile für: Disintegration                                                  #
# (C)opyright 1995 by Mark-Andre'Hopf                                           #
# (C)opyright 1995 by MAGIC INCLUDED RESEARCH                                   #
#                                                                               #
# dBASE is a (tm) of Borland International, Inc.                                #
#                                                                               #
#################################################################################

# Slackware Linux 1.1.59.

	CC						= gcc
	CFLAGS				= -ggdb
	EXT						= .cc

	PRGFILE				= 3d
	OBJS					= world.o render.o matrix.o
	LIBRARIES			= -lm -lX11
#	LIBRARIES			= -l$(DB_LIB) -ltk -ltcl -lm -lX11

	X11_INCLUDE		=	-I/usr/X11R6/include
	X11_LIBDIR		=	-L/usr/X11R6/lib -L/lib

#X11_INCLUDE		= -I/usr/openwin/include
#X11_LIBDIR		= -L/usr/openwin/lib

INCDIRS				= $(X11_INCLUDE)
LIBDIRS				= $(X11_LIBDIR)

all: $(PRGFILE)

#-----------------------------------------------------------------------------
# Umwandlungsvorschrift '*.cc' nach '*.o'
#-----------------------------------------------------------------------------

.SUFFIXES: $(EXT)

$(EXT).o:
	@echo compiling $*$(EXT) ...
	@$(CC) $(CFLAGS) $(INCDIRS) $*$(EXT) -c -o $*.o

#-----------------------------------------------------------------------------
# Linken
#-----------------------------------------------------------------------------
$(PRGFILE): $(OBJS)
	@echo linking $(PRGFILE) ...
	@gcc $(OBJS) -o $(PRGFILE) $(LIBDIRS) $(LIBRARIES)
	@echo Ok
