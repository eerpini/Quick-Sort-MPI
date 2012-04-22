#!/bin/bash
if [ $# -lt 2 ]; then
        echo "Usage : ./run.sh <num_machines> <num_elements>"
        exit 1
fi
machine_file="mfile"
if [ $(hostname) == "localhost.localdomain" ]; then
        machine_file="mfile.localhost"
else
        machine_file="mfile"
fi
#make clean && make
if [ $# -eq 3 ]; then
        mpirun -n $1 -machinefile $machine_file ./run $((2**$2)) $3
else
        mpirun -n $1 -machinefile $machine_file ./run $((2**$2))
fi
