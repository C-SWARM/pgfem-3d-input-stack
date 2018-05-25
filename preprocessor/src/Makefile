CC = g++ -fopenmp 
##CC = icpc -openmp
#CC = icpc -qopenmp

CFLAGS = -O3 -std=c++14 -g 

INCLUDES = -I../../input_lib/include -I../include

LIBS = -lm ../../input_lib/libpgfem3d_pipeline.a


PROGRAM = con3d++

SRCS =  options.cc main.cc domain.cc node.cc \
	element.cc boundary_conditions.cc initial_conditions.cc \
	material.cc header.cc basis.cc periodic.cc \
	apply_periodic_bc.cc process_input_files.cc \
	apply_header.cc write_pgfem_input.cc \
	renumber_global_nodes.cc comm_hints.cc \
	comm_hints_util.cc

OBJS = $(SRCS:.cc=.o)

all: $(OBJS) $(PROGRAM)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

tags:
	etags  $(SRCS)

clean:
	rm -rf *.o *~ $(PROGRAM) TAGS

#depend:
#	$(CC) $(CFLAGS) $(INCLUDES) -MM $(SRCS) > Makefile.depend

# DO NOT DELETE
#-include Makefile.depend
