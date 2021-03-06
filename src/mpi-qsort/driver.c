#include "header.h"


int validate(int* output, int globalNumElements) {
  int i = 0;
  assert(output != NULL);
  for(i = 0; i < globalNumElements - 1; i++) {
    if (output[i] > output[i+1]) {
      printf("************* NOT sorted *************\n");
      return 1;
    }
  }
  printf("============= SORTED ===========\n");
  return 0;
}

long long int timeval_diff(struct timeval *difference,
             struct timeval *end_time,
             struct timeval *start_time) {
  struct timeval temp_diff;

  if(difference == NULL){
    difference=&temp_diff;
  }

  difference->tv_sec = end_time->tv_sec - start_time->tv_sec ;
  difference->tv_usec = end_time->tv_usec - start_time->tv_usec;

  while(difference->tv_usec < 0){
    difference->tv_usec += 1000000;
    difference->tv_sec -= 1;
  }

  return 1000000LL * difference->tv_sec + difference->tv_usec;

} /* timeval_diff() */

int main(int argc, char **argv)
{
  // file pointers
  FILE* fin = NULL;
  FILE* fout = NULL;

  // variables
  int* globalInput = NULL;
  int* input = NULL;
  int* output = NULL;
  int globalNumElements, dataLength, sizeToRecv, i, j;
  MPI_Comm comm;
  int commRank, commSize;

  // time related
  long long diff;
  struct timeval starttime, endtime, timediff;
  if(argc == 2){
          argv[2] = (char *)malloc(21);
          sprintf(argv[2], "%d", time(NULL));
  }


  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &commRank );
  MPI_Comm_size( MPI_COMM_WORLD, &commSize );

  if (commRank == 0) {
          printf("Seed is : %s\n", argv[2]);
          srand(atol(argv[2]));
        globalNumElements = atoi(argv[1]);
        if( !(globalInput = (int *)calloc(globalNumElements, sizeof(int))) )
                {printf("Memory error\n"); exit(0);}

        for(i = 0; i < globalNumElements ;i++)
                globalInput[i] = rand()%1000000;

    if(i < globalNumElements)
      {printf("Invalid input\n"); exit(0);}
  }

  MPI_Bcast (&globalNumElements, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if( !(input = (int *)calloc(globalNumElements, sizeof(int))) )
    {printf("Memory error\n"); exit(0);}

  dataLength = globalNumElements / commSize;
  MPI_Scatter (globalInput, dataLength, MPI_INT, input, dataLength, MPI_INT, 0, MPI_COMM_WORLD);
  comm = MPI_COMM_WORLD;

  MPI_Barrier(MPI_COMM_WORLD);

  if (commRank == 0) {
    gettimeofday(&starttime,0x0);
  }

  // CALL TO SORT FUNCTION
  output = mpiqsort(input, globalNumElements, &dataLength, comm, commRank, commSize);

  MPI_Barrier(MPI_COMM_WORLD);

  if (commRank == 0) {
    gettimeofday(&endtime,0x0);
    j = 0;
    for(i = 1; i < commSize; i++) {
      MPI_Recv (&sizeToRecv, 1, MPI_INT, i, i, MPI_COMM_WORLD, NULL);
      MPI_Recv (globalInput + j, sizeToRecv, MPI_INT, i, i, MPI_COMM_WORLD, NULL);
      j += sizeToRecv;
    }
    if((fout = fopen("output.txt", "w")) == NULL) {
      printf("Error opening output file\n");
      exit(0);
    }
    fprintf(fout, "%d\n", globalNumElements);
    memcpy(output+dataLength, globalInput, (globalNumElements - dataLength)*sizeof(int));
    if(validate(output, globalNumElements) != 0){
            exit(EXIT_FAILURE);
    }

    diff = timeval_diff(&timediff,&endtime,&starttime);
    printf("Time to sort = %lld micro seconds\n", diff);
  }
  else {
    MPI_Send (&dataLength, 1, MPI_INT, 0, commRank, MPI_COMM_WORLD);
    MPI_Send (output, dataLength, MPI_INT, 0, commRank, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
