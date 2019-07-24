# T3dScale Makefile #

topdir		= ..
srcdir		= $(topdir)/src
incldir		= $(topdir)/include
bindir		= $(srcdir)

CC		= g++
CFLAGS		= -Wall
#CFLAGS		+= -DSC_DEBUG
#CFLAGS		+= -g
CFLAGS		+= -O3 #optimization

RM		= rm -rf

PROGRAM		= $(bindir)/T3dScale
OBJS		:= $(patsubst %.cpp,%.o,$(wildcard *.cpp))

INCLUDES	= -I$(incldir)
LIBS		=

all: $(OBJS) $(PROGRAM)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

$(PROGRAM): $(OBJS)
	$(RM) $(PROGRAM)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	$(RM) $(PROGRAM) *.o *~
