#include <arpa/inet.h>
#include <errno.h>
#include <libgonc/gonc_array.h>
#include <libgonc/gonc_list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_parent.h"
#include "gonm_socket_list.h"
#include "gonm_socket_list_array.h"

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

    struct gonm_socket_list_element* client_socket_list_element;
    for(size_t index = 0; ; index = (index + 1) % GONC_ARRAY_SIZE(parent_args->client_socket_list_array))
    {
        client_socket_list_element = malloc(sizeof(struct gonm_socket_list_element));
        GONC_LIST_ELEMENT_INIT(client_socket_list_element);
        if((client_socket_list_element->socket = accept(server_socket, NULL, NULL)) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            free(client_socket_list_element);
            continue;
        }
        pthread_mutex_lock(GONC_ARRAY_GET(parent_args->client_socket_mutex_array, index));
        GONC_LIST_APPEND(GONC_ARRAY_GET(parent_args->client_socket_list_array, index), client_socket_list_element);
        if(pthread_cond_signal(GONC_ARRAY_GET(parent_args->client_socket_cond_array, index)) != 0)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }
        pthread_mutex_unlock(GONC_ARRAY_GET(parent_args->client_socket_mutex_array, index));
    }
}

