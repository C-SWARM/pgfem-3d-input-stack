#!/bin/bash

# This script generates input files using the compiled executable and
# compares the results against a reference solution. If differences
# are found, they are printed to stdout and an error code is returned.
#
# USAGE:
#   ./reg-test.sh [clean]

basename="box_QT"
if [ $# -gt 0 ] && [ x"$1" == "xclean" ]; then
    echo "Cleaning old tests"
    rm ${basename}_*CPU/*.in
    exit 0
fi

CON3D_PATH=${PWD}/../../src/con3d++
topdir=${PWD}
procs=(1 2 4);
total_err=0;
for np in ${procs[*]}; do

    # change into the working directory for the number of processors
    workdir=${basename}_${np}CPU
    cd $topdir/$workdir

    # generate the input files
    $CON3D_PATH -np $np -f $basename

    # compare each generated file to its reference counterpart
    proc_err=0
    for in_file in *.in; do
	if [ in_file -ne *comm_hints*.in ]; then
        	diff -w $in_file ref/$in_file
	fi

        # get/store error status and report to stdout
        error=$?
        proc_err=$(( $proc_err + $error ))
        if [ $error != 0 ]; then
            echo "$workdir/$in_file FAILED!"
        else
            echo "$workdir/$in_file PASSED!"
        fi
    done

    # report error status for processor count
    if [ $proc_err != 0 ]; then
        echo "FAILED one or more tests for $np processors!"
    else
        echo "PASSED all tests for $np processors!"
    fi
    total_err=$(( $total_err + $proc_err ))
done

if [ $total_err != 0 ]; then
    echo "FAILED one or more tests!"
else
    echo "PASSED all tests!"
fi

exit $total_err

