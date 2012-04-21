make; make clean all
./generate 8388608
mpirun -n 8 -machinefile mfile run
