#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "header.h"


int validate (int *output, int num_elements)
{
	int i = 0;
	assert (output != NULL);
	for (i = 0; i < num_elements - 1; i++)
	  {
		  if (output[i] > output[i + 1])
		    {
			    printf ("************* NOT sorted *************\n");
                            printf("Element [%d]  = %d is greater than [%d] = %d \n", i, output[i], i+1, output[i+1]);
			    return -1;
		    }
	  }
	printf ("============= SORTED ===========\n");
        return 0;
}

int main (int argc, char **argv)
{
	FILE *fin = NULL;
	int *input = NULL;
	int *output = NULL;
	int num_elements, num_threads, i = 0;
        unsigned int timeval = 0;

	if (argc < 3)
	  {
		  printf ("Usage: ./pqsort <num of threads> <num of elements>\n");
	  }
        if( argc == 4){
                timeval = (unsigned int)atof(argv[3]);
        }


	num_threads = atoi (argv[1]);
        num_elements = atoi (argv[2]);

	if (!(input = (int *) calloc (num_elements, sizeof (int))))
	  {
		  printf ("Memory error\n");
		  exit (0);
	  }

        if(timeval == 0){
                timeval = time ( NULL );
        }
        printf("The seed is : %d\n", timeval);
        srand(timeval);
        for(i = 0; i < num_elements ;i++){
                input[i] = rand()%1000000;
        }
	output = pqsort (input, num_elements, num_threads);


#ifdef DEBUG_SORT
        for(i=0; i< num_elements; i++)
                printf("Output[%d] = %d\n", i, output[i]);
#endif

	if(validate (output, num_elements) != 0)
                return EXIT_FAILURE;
        //fclose(fin);
	return EXIT_SUCCESS;
}
