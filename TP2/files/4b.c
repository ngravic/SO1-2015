#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>   

#define N 10000000
#define N_PROCS 4

typedef struct {
    int *v;
    int b, t;
} QSParams;

void swap(int *v, int i, int j) 
{
    int tmp = v[i];
    v[i] = v[j];
    v[j] = tmp;
}

int colocar(int *v, int b, int t)
{
    int i;
    int pivote, valor_pivote;
    int temp;

    pivote = b;
    valor_pivote = v[pivote];
    for (i = b + 1; i <= t; i++){
        if (v[i] < valor_pivote){
            pivote++;    
            swap(v, i, pivote);
        }
    }
    temp = v[b];
    v[b] = v[pivote];
    v[pivote] = temp;
    return pivote;
} 
 
void *QuicksortSeq(void *p)
{
    QSParams *params = (QSParams *)p;
    int *v = params->v;
    int b = params->b, t = params->t;
    QSParams P1, P2;
    int pivote;
    if(b < t){
        pivote = colocar(v, b, t);
        P1.v = v;
        P1.b = b;
        P1.t = pivote - 1;
        P2.v = v;
        P2.b = pivote + 1;
        P2.t = t;
        QuicksortSeq((void *)&P1);
        QuicksortSeq((void *)&P2);
    }  
}

void QuicksortPar(int *v, int b, int t)
{
    QSParams Params[N_PROCS];
    pthread_t Threads[N_PROCS];
    int i, j;
    int pivotes[N_PROCS + 1];

    for (i = 0; i < N_PROCS - 1; i++) 
        pivotes[i + 1] = colocar(v, b, t);        

    pivotes[0] = b;
    pivotes[N_PROCS] = t;
    //Ordenar arreglo de pivotes
    for(i = 0; i < N_PROCS + 1; i++)    
        for(j = i; j < N_PROCS + 1; j++)        
            if(pivotes[i] > pivotes[j])
                swap(pivotes,i, j);

    for(i = 0; i < N_PROCS; i++) {
        Params[i].v = v;
        Params[i].b = pivotes[i];
        Params[i].t = pivotes[i + 1];
        pthread_create(&Threads[i], 0, QuicksortSeq, (void *)&Params[i]);
    }
    for (i = 0; i < N_PROCS; i++) {
        pthread_join(Threads[i], NULL);
    }
}

int main(int argc, char **argv) 
{
    int *a, i;

    a = malloc(N * sizeof(int));

    for(i = 0; i < N; i++) 
        a[i] = random() % N;

    QuicksortPar(a, 0, N-1);

    free(a);
    return 0;
}
