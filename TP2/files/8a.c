#include <stdio.h>
#include <sys/socket.h>       
#include <sys/types.h>       
#include <arpa/inet.h>      
#include <unistd.h>        
#include <string.h>        
#include <pthread.h>

#define BUFF_SIZE 1024
#define PORT 8000

pthread_mutex_t s = PTHREAD_MUTEX_INITIALIZER;  
int client_id= -1;

void *handle_client(void *arg)
{
    int conn_s = *(int *)arg;    
    char buffer[BUFF_SIZE], buffer2[BUFF_SIZE];
    int res;
    pthread_mutex_lock(&s);
    int CID = ++client_id; // Nuevo cliente
    pthread_mutex_unlock(&s);    
    fprintf(stderr, "New client %d connected\n", CID);
    while(1) {
        res = read(conn_s, buffer, BUFF_SIZE);
        if (res <= 0) {
            close(conn_s);
            fprintf(stderr, "Client disconnected\n");
            return;
        }
        buffer[res] = '\0';
        sprintf(buffer2, "Response to client %d: %s", CID, buffer);
        write(conn_s, buffer2, strlen(buffer2));
    }
}

int main()
{
    pthread_t t;
    int list_s, conn_s= -1, res;
    struct sockaddr_in servaddr;
    char buffer[BUFF_SIZE], buffer2[BUFF_SIZE];
    if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");
        return -1; 
    }

    if (listen(list_s, 10) < 0) {
        fprintf(stderr, "ECHOSERV: Error calling listen()\n");
        return -1;                          
    }

    while(1) {
        if ((conn_s = accept(list_s, NULL, NULL) ) < 0) {
            fprintf(stderr, "ECHOSERV: Error calling accept()\n");
            return -1;
        }
        else 
            pthread_create(&t, 0, handle_client, (void *)&conn_s);                
    }
    pthread_join(t, NULL);      
    return 0;
}
