#include <arpa/inet.h>
#include <errno.h>
#include <libgonc/gonc_array.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_parent.h"
#include "gonm_int_array.h"

void gonm_parent_start(struct gonm_parent_args* parent_args)
{
    int server_socket;
    struct sockaddr_in server_address;

    memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
    server_address.sin_family = AF_INET;
    inet_aton(parent_args->address, &server_address.sin_addr);
    server_address.sin_port = htons(parent_args->port);

    if((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(const int)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &(const int){1}, sizeof(const int)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(server_socket, parent_args->backlog) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int client_socket;
    for(size_t index = 0; ; index = (index + 1) % GONC_ARRAY_SIZE(parent_args->client_socket_array))
    {
        if((client_socket = accept(server_socket, NULL, NULL)) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            continue;
        }
        pthread_mutex_lock(GONC_ARRAY_GET(parent_args->client_socket_mutex_array, index));
        GONC_ARRAY_SET(parent_args->client_socket_array, index, client_socket);
        if(pthread_cond_signal(GONC_ARRAY_GET(parent_args->client_socket_cond_array, index)) != 0)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }
        pthread_mutex_unlock(GONC_ARRAY_GET(parent_args->client_socket_mutex_array, index));
    }
}

