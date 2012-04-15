all:
	mpicc mpi_pqsort.c -o sort -g -Wall
clean:
	rm -rf sort
