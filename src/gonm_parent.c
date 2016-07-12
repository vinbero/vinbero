#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "gonm_parent.h"

void gonm_parent_sigchld_handler(int signal_name)
{
    if(signal_name == SIGCHLD)
    {
        fprintf(stderr, "SIGCHLD!\n");
        int status;
        pid_t child_pid = waitpid(-1, &status, WNOHANG);
        if(WIFEXITED(status))
        {
            fprintf(stderr, "child %u eixted\n", child_pid);
            fprintf(stderr, "child exit status: %d\n", WEXITSTATUS(status));
        }
        else
        {
            fprintf(stderr, "Child abnormally exited");
        }
    }
}

ssize_t gonm_parent_send_client_socket(int child_socket, int client_socket)
{
    struct iovec io_vector[1];
    char io_vector_base[1];
    struct msghdr message_header;
    char message_control[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *control_message_header = NULL;

    memset(&message_header, 0, sizeof(struct msghdr));
    memset(message_control, 0, CMSG_SPACE(sizeof(int)));

    io_vector_base[0] = 0;
    io_vector[0].iov_base = io_vector_base;
    io_vector[0].iov_len = 1;

    message_header.msg_name = NULL;
    message_header.msg_namelen = 0;
    message_header.msg_iov = io_vector;
    message_header.msg_iovlen = 1;
    message_header.msg_controllen = CMSG_SPACE(sizeof(int));
    message_header.msg_control = message_control;

    control_message_header = CMSG_FIRSTHDR(&message_header);
    control_message_header->cmsg_level = SOL_SOCKET;
    control_message_header->cmsg_type = SCM_RIGHTS;
    control_message_header->cmsg_len = CMSG_LEN(sizeof(int));
    *((int*)CMSG_DATA(control_message_header)) = client_socket;

    return sendmsg(child_socket, &message_header, 0);
}

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
    for(size_t i = 0; ; i = (i + 1) % parent_args->child_socket_array->size)
    {
        if((client_socket = accept(server_socket, NULL, NULL)) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            continue;
        }

        if(gonm_parent_send_client_socket(parent_args->child_socket_array->elements[i], client_socket) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }

        if(close(client_socket) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }
    }
}

