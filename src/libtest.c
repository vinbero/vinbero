#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "gonm_string_list.h"

int gonm_module_run(int client_socket, struct gonm_string_list* module_path_list)
{
    write(client_socket, "HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer:\
 gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));

    if(close(client_socket) == -1)
    {
        fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
    }
}
