#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include "gonm_module.h"

int gonm_module_init(struct gonm_module_args* module_args, struct gonm_module_list* module_list)
{
    struct gonm_module* module = malloc(sizeof(struct gonm_module));
    GONC_LIST_ELEMENT_INIT(module);
    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int gonm_module_service(struct gonm_module* module, int client_socket)
{
    write(client_socket, "HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n", sizeof("HTTP/1.1 200 OK\r\nServer: gonm\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHELLO\r\n"));
    return 0;
}
