# makefile for tree stuff
# vix 24jul87 [stripped down from as makefile]
# vix 22jan93 [revisited; uses RCS, ANSI, POSIX; has bug fixes]
#
# $Id:$

#(choose a c compiler)
CC		=	gcc -Wall -Wtraditional -Wshadow -Wpointer-arith \
			-Wcast-align -Wwrite-strings -pedantic
#CC		=	cc

CDEBUG		=	-g
CFLAGS		=	$(CDEBUG) $(CCFLAGS)
LDFLAGS		=	$(CDEBUG)

TRTEST_OBJ	=	t_trtest.o tree.o

all		:	t_trtest

t_trtest	:	$(TRTEST_OBJ)
			$(CC) -o t_trtest $(TRTEST_OBJ)

lib: tree.o
	ar rv libtr.a tree.o

#Set up LIBDIR to point to the location that the lib will be placed.

LIBDIR= .
install:
	cp libtr.a $(LIBDIR)
#Set up MANDIR to point to a suitable location.
MANDIR= .
install.man:
	cp tree.man3 $(MANDIR)/tree.3

clean		:	FRC
			rm -f core a.out t_trtest $(TRTEST_OBJ)

FRC		:

tree.o		:	tree.c tree.h vixie.h
t_trtest.o	:	t_trtest.c tree.h vixie.h
