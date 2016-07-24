#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include "tcpcube_module.h"

int tcpcube_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list)
{
    struct tcpcube_module* module = malloc(sizeof(struct tcpcube_module));
    GONC_LIST_ELEMENT_INIT(module);
    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int tcpcube_module_service(struct tcpcube_module* module, int client_socket)
{
    write(client_socket, "HTTP/1.1 200 OK\r\nServer: tcpcube\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: tcpcube\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
    return 0;
}

int tcpcube_module_destroy(struct tcpcube_module* module)
{
    return 0;
}
