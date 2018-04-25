SRC = $(shell ls *.cc) 

OBJs = $(SRC:.cc=.o)

#EXC = exe
SETLIB = libpgfem3d_pipeline.a

CFLAGS = -Wall -std=c++14 -O3 -g 
CC = g++ 

AR = ar
ARFLAGS = -rcs

LIBS = 
INCs = -I./include -I./contrib/json-2.1.1/src

#$(EXC): $(OBJs)
#	$(CC) $(LFLAGS) -o $(EXC)  $(OBJs)  $(LIBS)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCs) -c $< 

all: $(OBJs)

#all: SRC

#SRC:
#	@echo $(OBJs)

	@echo $(OBJs)
  
	$(AR) $(ARFLAGS) $(SETLIB) $(OBJs)

clean:
	rm -f *.out *.o *.a core* exe
