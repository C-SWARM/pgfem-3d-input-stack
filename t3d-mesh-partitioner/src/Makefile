CC = gcc -fopenmp
#CC = icc -openmp
CFLAGS = -O3 -g
METIS = /opt/crc/scilib/metis/4.0/gnu
#METIS = /opt/crc/scilib/metis/4.0/intel

CKIT = ../contrib/Ckit
ELIXIR = ../contrib/Elixir

srcdir = .
prefix = .
exec_prefix = $(prefix)

LDFLAGS         = 
LDOPTIONS       = $(CDEBUGFLAGS) $(LDFLAGS)

LIBS            = -lm 
X_LIBS          =  -lSM -lICE  -L/usr/lib64 -lXaw -lXmu -lXt -lXext -lX11 

LOC_LIBS        = -L$(METIS) -lmetis -L$(ELIXIR)/src -lesi -lelixir -L$(CKIT)/src -lckit
SYS_LIBS        = $(X_LIBS) $(LIBS)

INCLUDES        = 
LOC_INCL	= -I$(METIS) -I$(CKIT)/include -I$(ELIXIR)/include

AR              = ar
AR_FLAGS        = rc
RANLIB          = ranlib

RM              = rm -f
DEPEND          = makedepend


#
# You probably don't need to change anything below here.
#

####################################################################
# List of object files 

OBJS  = t3d2psifel.o renumber.o

####################################################################
# List of source files
# 

SRCS  = t3d2psifel.c renumber.c

#########################################################################
#  Targets
#

all: t3d2psifel

t3d2psifel: $(OBJS) 
	$(RM) $@
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJS) $(LOC_LIBS) $(SYS_LIBS)

clean:
	$(RM) t3d2psifel *.o core

################################################################
# Dependencies
.c.o:
	$(CC) -c $(CFLAGS) $(DEFS) $(INCLUDES) $(LOC_INCL) $<

depend: $(SRCS)
	$(DEPEND) -DMAKEDEPEND $(DEFS) $(INCLUDES) $(LOC_INCL) $(SRCS)









