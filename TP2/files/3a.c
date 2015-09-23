#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define NUM_PROC 2
#define N 50000000

int A[N];

int search_min_seq(int *array, int arr_size)
{
    int min = array[0];    
    for(i = 1; i < arr_size; i++)
        if(array[i] < min) min = array[i];
    return min;
}

int search_min_parallel(int *array, int arr_size)
{
    int min = array[0];
    #pragma omp parallel for
    for(i = 1; i < arr_size; i++)
        if(array[i] < min) min = array[i];
    return min;
}

int main()
{
	omp_set_num_threads(NUM_PROC);
    int i;
    for(i = 0; i < N; i++) 
        A[i] = random() % 50000;

    double start = omp_get_wtime();
    printf("Minimo paralelo: %d\n", search_min_parallel(A, N));
    double end = omp_get_wtime();
    printf("Tiempo paralelo: %f\n", end - start);
    start = omp_get_wtime();
    printf("Minimo secuencial: %d\n", search_min_seq(A, N));
    end = omp_get_wtime();
    printf("Tiempo secuencial: %f\n", end - start);    
    return 0;
}

