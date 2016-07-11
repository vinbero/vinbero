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

int gonm_parent_send_client_socket(int child_socket, int client_socket)
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
    *((int *) CMSG_DATA(control_message_header)) = client_socket;

    if(sendmsg(child_socket, &message_header, 0) == -1)
        return -1;
    return 0;
}

void gonm_parent_start(struct gonm_socket_array* child_socket_array)
{
    int server_socket;
    struct sockaddr_in server_address;
    memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
    server_address.sin_family = AF_INET;
    inet_aton("0.0.0.0", &server_address.sin_addr);
    server_address.sin_port = htons(8080);
    if((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    bool is_socket_reusable = true;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void*)is_socket_reusable, sizeof(is_socket_reusable));
    if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(server_socket, 1024) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_length;
    for(size_t i = 0; ; i = (i + 1) % child_socket_array->size)
    {
        if((client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length)) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            continue;
        }
        if(gonm_parent_send_client_socket(child_socket_array->elements[i], client_socket) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }
    }
}

