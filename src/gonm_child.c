#include <dlfcn.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gonm_child.h"
#include "gonm_string_list.h"

int gonm_child_receive_client_socket(int parent_socket)
{
    int client_socket;
    struct msghdr message_header;
    struct iovec io_vector[1];
    struct cmsghdr *control_message_header = NULL;
    char message_control[CMSG_SPACE(sizeof(int))];
    char io_vector_base[1];

    memset(&message_header, 0, sizeof(struct msghdr));
    memset(message_control, 0, CMSG_SPACE(sizeof(int)));

    io_vector[0].iov_base = io_vector_base;
    io_vector[0].iov_len = 1;

    message_header.msg_name = NULL;
    message_header.msg_namelen = 0;
    message_header.msg_control = message_control;
    message_header.msg_controllen = CMSG_SPACE(sizeof(int));
    message_header.msg_iov = io_vector;
    message_header.msg_iovlen = 1;

    ssize_t message_length = recvmsg(parent_socket, &message_header, 0);
    if(message_length <= 0)
        return message_length;

    control_message_header = CMSG_FIRSTHDR(&message_header);
    if(control_message_header->cmsg_level == SOL_SOCKET && control_message_header->cmsg_type == SCM_RIGHTS && control_message_header->cmsg_len == CMSG_LEN(sizeof(int)))
        return *((int*)CMSG_DATA(control_message_header));

    return -1;
}

void gonm_child_start(int parent_socket, struct gonm_string_list* module_path_list)
{
    int (*gonm_module_run)(int, struct gonm_string_list*);
    void* dl_handle;

    if((dl_handle = dlopen(module_path_list->first->string, RTLD_LAZY)) == NULL)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if((gonm_module_run = dlsym(dl_handle, "gonm_module_run")) == NULL)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, dlerror());
        exit(EXIT_FAILURE);
    }
    
    for(int client_socket = gonm_child_receive_client_socket(parent_socket); ; client_socket = gonm_child_receive_client_socket(parent_socket))
    {
        if(client_socket == 0)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, "Parent abnormally exited");
            exit(EXIT_FAILURE);
        }
        else if(client_socket == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
            continue;
        }

        gonm_module_run(client_socket, module_path_list);
/*
        write(client_socket, "HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
        
        if(close(client_socket) == -1)
        {
            fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
        }
*/
    }
    dlclose(dl_handle);
}
