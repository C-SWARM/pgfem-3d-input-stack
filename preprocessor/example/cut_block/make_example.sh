#!/bin/bash

# simple script to make the example for N processes.
if [ $# -lt 1 ]; then
    echo "Must provide number of processes"
    exit
fi

nproc=$1

filebase="cohe_block"
t3d_options="-d 0.1 -r 1 -p 8"; #'-\$ -X';
decomposition_options='-sp'

rm -rf $filebase.out $filebase.out.[0123456789]*

t3d -i $filebase.t3d -o $filebase.out $t3d_options
if [ $nproc -gt 1 ]; then
    t3d2psifel -np $nproc $filebase.out $filebase.out $decomposition_options
else
    cp -sf $filebase.out $filebase.out.0
fi

../../src/con3d++ -np $nproc -f $filebase -coh -ch -nt 1