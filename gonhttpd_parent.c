#include <arpa/inet.h>
#include <libgonc/gonc_list.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "gonhttpd_parent.h"

void gonhttpd_parent_sigchld_handler(int signal_name)
{
    if(signal_name == SIGCHLD)
    {
        printf("SIGCHLD!\n");
        int status;
        pid_t child_pid = waitpid(-1, &status, WNOHANG);
        if(WIFEXITED(status))
        {
            printf("child %u eixted\n", child_pid);
            printf("child exit status: %d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("Child abnormally exited");
        }
    }
}

void gonhttpd_parent_start(struct gonhttpd_socket_list* child_socket_list)
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length;
    memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
    server_address.sin_family = AF_INET;
    inet_aton("0.0.0.0", &server_address.sin_addr);
    server_address.sin_port = htons(8080);
    if((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        fprintf(stderr, "socket error\n");
        exit(1);
    }
    bool is_socket_reusable = true;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void*)is_socket_reusable, sizeof(is_socket_reusable));
    if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
        fprintf(stderr, "bind error\n");
        exit(1);
    }
    if(listen(server_socket, 10) == -1)
    {
        fprintf(stderr, "listen error\n");
        exit(1);
    }

    while(true)
    {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        printf("client_address_length: %u\n", client_address_length);
        if(client_socket == -1)
            fprintf(stderr, "accept error\n");
        write(client_socket, "HTTP/1.1 200 OK\r\nServer: gonhttpd\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: gonhttpd\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
        close(client_socket);

    }
}

