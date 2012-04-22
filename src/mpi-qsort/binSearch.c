#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef int elem_type;
#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }
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



int binarySearch( int * input, int start, int stop, int pivot){
        int j;
        int nelems = stop;
         do{
                if(start  == stop-1){
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
                        j=(start+stop)/2 + 1;
                        do{
                                if(input[j] != pivot){
                                        return j; 
                                }
                                j++;
                                //printf("InnerWhile\n");
                        }while(j<nelems);
                        return j;
                }
                //printf("OuterWhile start = %d stop = %d\n", start, stop);
        }while(1);
}


int compare (const void *a, const void *b)
{
	return (*(int *) a - *(int *) b);
}
int main(){
        int val = 4194304;
        int *input = malloc(val*sizeof(int));
        int i;
        int j;
        int pivot;
        srand(time(NULL));
        for(i = 0; i< val; i++){
                input[i] = rand()%1000000;
        }
        //pivot = median(input, 8192);
        pivot = input[0];
        qsort((void *)(input), (size_t)(val), (size_t)(sizeof(int)), compare);
        printf("Quick sort done\n");
        i=0;

        do{
                if(input[i] > pivot){
                        break;
                }
                i++;
        }while(i < val);
        printf("Linear Search done\n");
        j = binarySearch(input, 0 , val, pivot);
        printf(" pivot %d i [%d] j [%d]\n", pivot, i, j);
        printf(" input[i-1] %d input[i] %d input[i+1] %d input[j-1] %d input[j] %d input[j+1] %d \n", 
                        input[i-1],
                        input[i],
                        input[i+1],
                        input[j-1],
                        input[j],
                        input[j+1]);
        if(i!=j)
                return EXIT_FAILURE;
        return EXIT_SUCCESS;

}


