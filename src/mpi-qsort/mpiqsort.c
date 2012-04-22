#include "header.h"

typedef int elem_type ;

#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }


/*---------------------------------------------------------------------------
   Function :   kth_smallest()
   In       :   array of elements, # of elements in the array, rank k
   Out      :   one element
   Job      :   find the kth smallest element in the array
   Notice   :   use the median() macro defined below to get the median. 

                Reference:

                  Author: Wirth, Niklaus 
                   Title: Algorithms + data structures = programs 
               Publisher: Englewood Cliffs: Prentice-Hall, 1976 
    Physical description: 366 p. 
                  Series: Prentice-Hall Series in Automatic Computation 

 ---------------------------------------------------------------------------*/


elem_type kth_smallest(elem_type a[], int n, int k)
{
    register i,j,l,m ;
    register elem_type x ;

    l=0 ; m=n-1 ;
    while (l<m) {
        x=a[k] ;
        i=l ;
        j=m ;
        do {
            while (a[i]<x) i++ ;
            while (x<a[j]) j-- ;
            if (i<=j) {
                ELEM_SWAP(a[i],a[j]) ;
                i++ ; j-- ;
            }
        } while (i<=j) ;
        if (j<k) l=i ;
        if (k<i) m=j ;
    }
    return a[k] ;
}


#define median(a,n) kth_smallest(a,n,(((n)&1)?((n)/2):(((n)/2)-1)))


int compare (const void *a, const void *b)
{
	return (*(int *) a - *(int *) b);
}

/*
 * Binary Search : array, start_index, num_elements, pivot
 */
int binarySearch( int * input, int start, int stop, int pivot){
        int j;
        int nelems = stop;
         do{
                if(start  == stop-1){
#ifdef BSEARCH_DEBUG
                       printf("1 Returnig %d and pivot is %d and element is %d element -1 is %d\n", start, pivot, input[start], input[start-1]);
#endif
                       if(input[start] <= pivot){
                               return start+1;
                       }
                       return start; 
                }
                if(input[(start+stop)/2] > pivot){
                        stop = (start+stop)/2;
                        //printf("OuterWhile start = %d stop = %d\n", start, stop);
                        continue;
                }
                if(input[(start+stop)/2] < pivot){
                        start = (start+stop)/2+1;
                        //printf("OuterWhile start = %d stop = %d\n", start, stop);
                        continue;
                }
                if(input[(start+stop)/2] == pivot){
                        j=(start+stop)/2; 
                        do{
                                j++;
                                //printf("InnerWhile\n");
                        }while(input[j] == pivot && j<nelems);
#ifdef BSEARCH_DEBUG
                        printf("Returnig %d and pivot is %d and element is %d\n", j, pivot, input[j]);
#endif
                        return j;
                }
                //printf("OuterWhile start = %d stop = %d\n", start, stop);
        }while(1);
}

int* mpiqsort_recur(int* input, int* dataLengthPtr, MPI_Comm comm, int commRank, int commSize, int *recvBuf, int *mergeBuf) {
        int pivot;
        /*
         * Rank of the person I have to send to or receive from
         */
        int dataEndPoint;
        int dataTransferSize;
        int recvdSize;
        int i, j;
        int origSize = *dataLengthPtr;
        int localMedian;
        struct timeval sttime, sptime;

        int tempMedians[commSize];
        int _incr = 1;
        if(*dataLengthPtr <= 0){
                fprintf(stderr, "We got 0 elements, this should not happen\n");
                return NULL;
        }
        localMedian = median(input, *dataLengthPtr);
        tempMedians[commRank] = localMedian;

        MPI_Gather(&localMedian, 1, MPI_INT, tempMedians, 1, MPI_INT, 0, comm);
#ifdef DEBUG
        printf("[%d] Done with Gather\n", commRank);
#endif

        if(commRank == 0){
                pivot = median(tempMedians, commSize); 
#ifdef DEBUG
                printf("Got the pivot in root : [%d]\n", pivot);
#endif
        }
#ifdef DEBUG
        printf("Sending the pivot to everyone\n");
#endif
        MPI_Bcast(&pivot, 1, MPI_INT, 0, comm);
#ifdef DEBUG
        printf("I have pivot : %d\n", pivot);
#endif


        qsort((void *)(input), (size_t)(*dataLengthPtr), (size_t)(sizeof(int)), compare);
#ifdef DEBUG
        printf("Sorting done \n");
#endif
        if(commSize == 1){
                return input;
        }

#ifdef BSEARCH_DEBUG
        i=0;

        gettimeofday(&sttime, 0x0);
        
        do{
                if(input[i] > pivot)
                        break;
                i++;

        }while(i < *dataLengthPtr );
        gettimeofday(&sptime, 0x0);
        printf("The time taken for serial lookup : %lld\n",timeval_diff(NULL, &sptime, &sttime)); 
        printf("i is : %d\n", i);
        gettimeofday(&sttime, 0x0);
        j = binarySearch(input, 0, *dataLengthPtr, pivot);
        printf("COMPARISON : j [%d] i [%d]\n", j, i);
        gettimeofday(&sptime, 0x0);
        printf("The time taken for binary : %lld\n",timeval_diff(NULL, &sptime, &sttime)); 
        i=j;
#endif
#ifdef DEBUG
        gettimeofday(&sttime, 0x0);
#endif
        i = binarySearch(input, 0, *dataLengthPtr, pivot);
#ifdef DEBUG
        gettimeofday(&sptime, 0x0);
        printf("The time taken for binary : %lld\n",timeval_diff(NULL, &sptime, &sttime)); 
#endif
      
        /*
        j = binarySearch(input, 0, *dataLengthPtr-1, pivot);
        if(i!=j){
                printf("ERROR : Binary Search Failed , i[%d], binarySearch[%d] input[i] = [%d] input[j] = [%d]\n", i, j, input[i], input[j]);
                printf("INFO : pivot : [%d] j: [%d] input[j-1] : [%d] input[j] : [%d] input[j+1] : [%d] length [%d]\n", pivot, j, input[j-1], input[j], input[j+1], *dataLengthPtr);
                if(i< j){
                        for(; i<= j; i++){
                                printf("i[%d]= [%d]\n", i, input[i]);
                        }
                }
                else{
                        for(; j<= i; j++){
                                printf("j[%d]= [%d]\n", j, input[j]);
                        }
                }

                exit(EXIT_FAILURE);
        }
        */

        if(i == *dataLengthPtr){
                fprintf(stderr, "We could not locate the pivot in the sorted array, this should not happen\n");
        }

#ifdef DEBUG
        printf("Got the partitioning point\n");
#endif
        if(commRank >= commSize/2){
                /*
                 * We should be receiving first
                 */
                dataEndPoint = (commRank + commSize/2)%commSize;
#ifdef DEBUG
                printf("Receiving the size[%d] from endpoint[%d]\n", dataTransferSize, dataEndPoint);
#endif
                MPI_Recv(&dataTransferSize, 1, MPI_INT, dataEndPoint, dataEndPoint, comm, NULL);
#ifdef DEBUG
                printf("The size I am about to recieve from endpoint [%d] is [%d]\n", dataEndPoint, dataTransferSize);
#endif
                if(dataTransferSize != 0){
#ifdef DEBUG
                        printf("Now receiving the data from endpoint[%d]\n", dataEndPoint);
#endif
                        MPI_Recv(recvBuf, dataTransferSize, MPI_INT, dataEndPoint, dataEndPoint, comm, NULL);
                
                }
                recvdSize = dataTransferSize;
                *dataLengthPtr += recvdSize;
                /*
                 * Now we need to Send the first half
                 */
                dataTransferSize = i;
                MPI_Send(&dataTransferSize, 1, MPI_INT, dataEndPoint, commRank, comm);
                if(dataTransferSize != 0){
                        MPI_Send(input, dataTransferSize, MPI_INT, dataEndPoint, commRank, comm);
                }
                *dataLengthPtr -= dataTransferSize;
                /*
                 * We need to merge here, we have the second half and the received second half in recvBuf
                 */
                dataTransferSize = i;
                i = 0;
                j=0;
                while(1){
                        if(i>= *dataLengthPtr)
                                break;
                        if(j>= recvdSize){
                                mergeBuf[i] = input[dataTransferSize];
                                dataTransferSize++;
                                i++;
                                continue;
                        }
                        if(dataTransferSize >= origSize){
                                mergeBuf[i] = recvBuf[j];
                                j++;
                                i++;
                                continue;
                        }
                        if(input[dataTransferSize] <= recvBuf[j]){
                                mergeBuf[i] = input[dataTransferSize];
                                dataTransferSize++;
                                i++;
                                continue;
                        }
                        else{
                                mergeBuf[i] = recvBuf[j];
                                j++;
                                i++;
                                continue;
                        }
                }

                        

        }
        else{
                /*
                 * We should be sending first the second half
                 */
                dataTransferSize = *dataLengthPtr - i;
                //*dataLengthPtr = i;
                dataEndPoint = (commRank + commSize/2)%commSize;
#ifdef DEBUG
                printf("Sending the size[%d] to endpoint[%d]\n", dataTransferSize, dataEndPoint);
#endif
                MPI_Send(&dataTransferSize, 1, MPI_INT, dataEndPoint, commRank, comm);
                if(dataTransferSize != 0){
#ifdef DEBUG
                        printf("Now sending the data to endpoint[%d]\n", dataEndPoint);
#endif
                        MPI_Send((input + i), dataTransferSize, MPI_INT, dataEndPoint, commRank, comm);
                }
                *dataLengthPtr -= dataTransferSize;
                /*
                 * Now we need to receive the first half
                 */
                MPI_Recv(&dataTransferSize, 1, MPI_INT, dataEndPoint, dataEndPoint, comm, NULL);
                if(dataTransferSize != 0){
                        MPI_Recv(recvBuf, dataTransferSize, MPI_INT, dataEndPoint, dataEndPoint, comm, NULL);
                }
                recvdSize = dataTransferSize;
                *dataLengthPtr += recvdSize;
                /*
                 * We need to merge here, we have the first half and the received first half in recvBuf
                 */
                origSize = i;
                dataTransferSize = 0;
                i = 0;
                j=0;
                while(1){
                        if(i>= *dataLengthPtr)
                                break;
                        if(j>= recvdSize){
                                mergeBuf[i] = input[dataTransferSize];
                                dataTransferSize++;
                                i++;
                                continue;
                        }
                        if(dataTransferSize >= origSize){
                                mergeBuf[i] = recvBuf[j];
                                j++;
                                i++;
                                continue;
                        }
                        if(input[dataTransferSize] <= recvBuf[j]){
                                mergeBuf[i] = input[dataTransferSize];
                                dataTransferSize++;
                        }
                        else{
                                mergeBuf[i] = recvBuf[j];
                                j++;
                        }

                        i++;
                        continue;
                }
        }

        /*
         * We should have been done with all the merging by now, so we block here for all the processes in 
         * the comm, the first process in the comm does the split
         */
        int color = 0;
        if(commRank >= commSize/2){
                color = 1;
        }
        else{
                color = 0;
        }
        MPI_Barrier(comm);
        /*
         * FIXME : Check if passing the same comm object twice can cause problems
         */
        MPI_Comm_split(comm, color, 1, &comm);
        MPI_Comm_size(comm, &commSize);
        MPI_Comm_rank(comm, &commRank);

        return mpiqsort_recur(mergeBuf, dataLengthPtr, comm, commRank, commSize, recvBuf, input);

}

int* mpiqsort(int* input, int globalNumElements, int* dataLengthPtr, MPI_Comm comm, int commRank, int commSize) {

        int *recvBuf = NULL;
        int *mergeBuf = NULL;
        if( !(recvBuf= (int *)calloc(globalNumElements, sizeof(int))) ){
                        printf("Memory error\n"); 
                        exit(0);
        }
        if( !(mergeBuf= (int *)calloc(globalNumElements, sizeof(int))) ){
                        printf("Memory error\n"); 
                        exit(0);
        }

#ifdef DEBUG
        printf("Calling recursive function here\n");
#endif
        return mpiqsort_recur(input , dataLengthPtr, comm, commRank, commSize, recvBuf, mergeBuf);

}

