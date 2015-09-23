#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CHAIRS 8
#define MAX_CUSTOMERS 10

pthread_cond_t is_waiting[MAX_CUSTOMERS];
pthread_cond_t is_haircutting = PTHREAD_COND_INITIALIZER;
pthread_mutex_t semaphore = PTHREAD_MUTEX_INITIALIZER;
pthread_t t1, t2[MAX_CUSTOMERS];

int args[MAX_CUSTOMERS];
int next = 0;   // Proximo en la cola para ser atendido
int last = 0;   // Ultima persona en llegar
int waiting_customers_qty;   // Cantidad de clientes en espera
int seats[MAX_CHAIRS];
int customer_status[MAX_CUSTOMERS];  // Estado de los clientes 

void *customer(void *arg){
    int CID = *(int*)arg;
    for(;;){
        sleep(random() % 3);
        pthread_mutex_lock(&semaphore);
        printf("Entra cliente %d\n", CID);
        if(waiting_customers_qty == MAX_CUSTOMERS)
            printf("Barberia llena. cliente %d se retira\n", CID);
        else{
            waiting_customers_qty += 1;
            seats[last] = CID;
            if(++last >= MAX_CUSTOMERS) 
                last = 0;
            customer_status[CID] = 1;
            printf("Cliente %d ocupa el asiento %d\n", CID, last);                    
            pthread_cond_signal(&is_haircutting);          
            while(customer_status[CID])
                pthread_cond_wait(&is_waiting[CID], &semaphore);
            printf("Cliente %d se retira\n", CID);
        }
        pthread_mutex_unlock(&semaphore);
    }
    return NULL;
}

void *barber(void *arg){
    for(;;) {
        sleep(random() % 3);
        pthread_mutex_lock(&semaphore);
        
        while(waiting_customers_qty == 0)
            pthread_cond_wait(&is_haircutting, &semaphore);
        printf("Barbero comienza a atender\n");
        
        int id = seats[next];
        printf("El barbero corta pelo a cliente %d\n", id);
        
        customer_status[id] = 0;
        waiting_customers_qty -= 1;
        if (++next >= MAX_CHAIRS) 
            next = 0;
        pthread_cond_signal(&is_waiting[id]);
        pthread_mutex_unlock(&semaphore);
    }
    return NULL;
}

int main(){
    int i;
    
    pthread_create(&t1, NULL, barber, NULL);
    for(i = 0; i < MAX_CUSTOMERS; i++){
        args[i] = i;
        pthread_cond_init(&is_waiting[i], 0);
        pthread_create(&t2[i], NULL, customer, (void*)&args[i]);
    }
    pthread_join(t1, NULL);

    return 0;
}