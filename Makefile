all:
	mpicc mpi_pqsort.c -o sort -g -Wall -Werror
clean:
	rm -rf sort
