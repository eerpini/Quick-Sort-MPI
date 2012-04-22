#!/bin/bash
if [ $# -lt 2 ]; then
        echo "Usage : ./run.sh <num_machines> <num_elements>"
        exit 1
fi
make clean && make
sleep 2
mpirun -n $1 -machinefile mfile ./run $2
