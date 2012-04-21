#include "header.h"

int compare (const void *a, const void *b)
{
	return (*(int *) a - *(int *) b);
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
        int splitRanks[commSize/2];
        if(*dataLengthPtr <= 0){
                fprintf(stderr, "We got 0 elements, this should not happen\n");
                return NULL;
        }
        if(commRank == 0){
                pivot = input[0];
        }
        printf("Sending the pivot to everyone\n");
        MPI_Bcast(&pivot, 1, MPI_INT, 0, comm);
        printf("I have pivot : %d\n", pivot);


        qsort((void *)(input), (size_t)(*dataLengthPtr), (size_t)(sizeof(int)), compare);
        printf("Sorting done \n");
        if(commSize == 1){
                return input;
        }

        i=0;
        do{
                if(input[i] > pivot)
                        break;
                i++;
        }while(i < *dataLengthPtr);

        if(i == *dataLengthPtr){
                fprintf(stderr, "We could not locate the pivot in the sorted array, this should not happen\n");
        }

        //*dataLengthPtr = (*dataLengthPtr - i);

        printf("Got the partitioning point\n");
        if(commRank >= commSize/2){
                /*
                 * We should be receiving first
                 */
                dataEndPoint = (commRank + commSize/2)%commSize;
                printf("Receiving the size[%d] from endpoint[%d]\n", dataTransferSize, dataEndPoint);
                MPI_Recv(&dataTransferSize, 1, MPI_INT, dataEndPoint, dataEndPoint, comm, NULL);
                printf("The size I am about to recieve from endpoint [%d] is [%d]\n", dataEndPoint, dataTransferSize);
                if(dataTransferSize != 0){
                        printf("Now receiving the data from endpoint[%d]\n", dataEndPoint);
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
                printf("Sending the size[%d] to endpoint[%d]\n", dataTransferSize, dataEndPoint);
                MPI_Send(&dataTransferSize, 1, MPI_INT, dataEndPoint, commRank, comm);
                if(dataTransferSize != 0){
                        printf("Now sending the data to endpoint[%d]\n", dataEndPoint);
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

        printf("Calling recursive function here\n");
        return mpiqsort_recur(input , dataLengthPtr, comm, commRank, commSize, recvBuf, mergeBuf);

}

