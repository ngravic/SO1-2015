#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define N       2
#define ARRLEN  1024

int arr[ARRLEN];

pthread_mutex_t semaphore = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_write = PTHREAD_COND_INITIALIZER;

int n_reading = 0, n_writing = 0;

void *escritor(void *arg){
    int i;
    int num = *((int *)arg);
    for (;;) {
        sleep(random() % 3);
        pthread_mutex_lock(&semaphore);
        n_writing = 1;
        while(n_reading)
            pthread_cond_wait(&can_write, &semaphore);            
            for (i = 0; i < ARRLEN; i++) {
                arr[i] = num;
            }
        n_writing = 0;
        pthread_cond_broadcast(&can_read);
        pthread_mutex_unlock(&semaphore);
    }
    return NULL;
}

void *lector(void *arg)
{
    int v, i, err;
    int num = *((int *)arg);
    for (;;) {
        sleep(random() % 3);
        pthread_mutex_lock(&semaphore);
        while(n_writing)
            pthread_cond_wait(&can_read, &semaphore);
        n_reading++;              
        pthread_mutex_unlock(&semaphore);       
        err = 0;
        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v) {
                err = 1;
                break;
            }
        }
        if (err) printf("Lector %d, error de can_read\n", num);
        else printf("Lector %d, dato %d\n", num, v);
        pthread_mutex_lock(&semaphore);        
        if(!(--n_reading))
            pthread_cond_signal(&can_write);
        pthread_mutex_unlock(&semaphore);
    }
    return NULL;
}

int main(){
    int i;
    pthread_t readers[N], writers[N];
    int arg[N];

    for (i = 0; i < ARRLEN; i++){
        arr[i] = -1;
    }
    for (i = 0; i < N; i++){
        arg[i] = i;
        pthread_create(&readers[i], NULL, lector, (void *)&arg[i]);
        pthread_create(&writers[i], NULL, escritor, (void *)&arg[i]);        
    }
    pthread_join(readers[0], NULL);

    return 0;
}

