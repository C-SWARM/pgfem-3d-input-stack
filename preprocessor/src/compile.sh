#!/bin/csh

module load mvapich2/2.2-gcc-7.1.0-mlx 

make clean
make -j 8
