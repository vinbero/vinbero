#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gonm_master.h"

void gonm_master_start(struct gonm_master_args* master_args)
{
    struct sockaddr_in server_address;

    memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
    server_address.sin_family = AF_INET;
    inet_aton(master_args->address, &server_address.sin_addr);
    server_address.sin_port = htons(master_args->port);

    if((master_args->worker_args->server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(setsockopt(master_args->worker_args->server_socket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(const int)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(setsockopt(master_args->worker_args->server_socket, SOL_SOCKET, SO_REUSEPORT, &(const int){1}, sizeof(const int)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(bind(master_args->worker_args->server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(master_args->worker_args->server_socket, master_args->backlog) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_t* threads = malloc(master_args->worker_count * sizeof(pthread_t));
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    master_args->worker_args->server_socket_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(master_args->worker_args->server_socket_mutex, NULL);    
    for(size_t index = 0; index != master_args->worker_count; ++index)
    {
        if(pthread_create(threads + index, &thread_attr, gonm_worker_start, master_args->worker_args) != 0)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    while(true){sleep(1);}
}

