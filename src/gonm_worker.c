#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_worker.h"

void* gonm_worker_start(void* worker_args)
{
    for(int client_socket;;)
    {
        pthread_mutex_lock(((struct gonm_worker_args*)worker_args)->server_socket_mutex);
        if((client_socket = accept(((struct gonm_worker_args*)worker_args)->server_socket, NULL, NULL)) == -1)
        {
            pthread_mutex_unlock(((struct gonm_worker_args*)worker_args)->server_socket_mutex);
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            continue;
        }
        pthread_mutex_unlock(((struct gonm_worker_args*)worker_args)->server_socket_mutex);
        write(client_socket, "HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
        
        if(close(client_socket) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }
    }
    return NULL;
}
