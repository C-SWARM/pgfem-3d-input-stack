#!/bin/bash

# This script generates input files using the compiled executable and
# compares the results against a reference solution. If differences
# are found, they are printed to stdout and an error code is returned.
#
# USAGE:
#   ./reg-test.sh [clean]

basename="box"
if [ $# -gt 0 ] && [ x"$1" == "xclean" ]; then
    echo "Cleaning old tests"
    rm ${basename}_8CPU_*thread/$basename.out.*
    exit 0
fi

CON3D_PATH=${PWD}/../../src/t3d2psifel
topdir=${PWD}
procs=(1 2 8);
nproc=8;
total_err=0;
for nt in ${procs[*]}; do

    # change into the working directory for the number of processors
    workdir=${basename}_8CPU_${nt}thread
    cd $topdir/$workdir

    # generate the input files
    $CON3D_PATH -np $nproc $basename.out $basename.out -nt $nt -sp

    # compare each generated file to its reference counterpart
    proc_err=0
    for out_file in $basename.out.*; do
        diff -w $out_file ref/$out_file

        # get/store error status and report to stdout
        error=$?
        proc_err=$(( $proc_err + $error ))
        if [ $error != 0 ]; then
            echo "$workdir/$out_file FAILED!"
        else
            echo "$workdir/$out_file PASSED!"
        fi
    done

    # report error status for processor count
    if [ $proc_err != 0 ]; then
        echo "FAILED one or more tests for $nt threads!"
    else
        echo "PASSED all tests for $nt threads!"
    fi
    total_err=$(( $total_err + $proc_err ))
done

if [ $total_err != 0 ]; then
    echo "FAILED one or more tests!"
else
    echo "PASSED all tests!"
fi

exit $total_err
