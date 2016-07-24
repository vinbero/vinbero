#include <arpa/inet.h>
#include <dlfcn.h>
#include <err.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include "config.h"
#include "tcpcube_master.h"
#include "tcpcube_worker.h"

void tcpcube_master_start(struct tcpcube_master_args* master_args)
{
    struct tcpcube_worker_args* worker_args = malloc(sizeof(struct tcpcube_worker_args));
    struct sockaddr_in server_address;
    memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
    server_address.sin_family = AF_INET;
    inet_aton(master_args->address, &server_address.sin_addr);
    server_address.sin_port = htons(master_args->port);

    if((worker_args->server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(worker_args->server_socket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(worker_args->server_socket, SOL_SOCKET, SO_REUSEPORT, &master_args->reuse_port, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(bind(worker_args->server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(listen(worker_args->server_socket, master_args->backlog) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    pthread_t* threads = malloc(master_args->worker_count * sizeof(pthread_t));
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    worker_args->server_socket_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(worker_args->server_socket_mutex, NULL);

    void* dl_handle;
    if((dl_handle = dlopen(GONC_LIST_HEAD(master_args->module_args_list)->module_path.chars, RTLD_LAZY)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    int (*tcpcube_module_init)(struct tcpcube_module_args*, struct tcpcube_module_list*);
    if((tcpcube_module_init = dlsym(dl_handle, "tcpcube_module_init")) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((worker_args->tcpcube_module_start = dlsym(dl_handle, "tcpcube_module_start")) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    int (*tcpcube_module_destroy)(struct tcpcube_module*);
    if((tcpcube_module_destroy = dlsym(dl_handle, "tcpcube_module_destroy")) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    struct tcpcube_module_list* module_list = malloc(sizeof(struct tcpcube_module_list));
    GONC_LIST_INIT(module_list);

    if(tcpcube_module_init(GONC_LIST_HEAD(master_args->module_args_list), module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tcpcube_module_init() failed.", __FILE__, __LINE__);

    GONC_LIST_FOR_EACH(master_args->module_args_list, struct tcpcube_module_args, module_args)
    {
        GONC_LIST_FOR_EACH(module_args, struct tcpcube_module_arg, module_arg)
        {
            free(module_arg->name.chars);
            free(module_arg->value.chars);
            free(module_arg);
        }
        free(module_args->module_path.chars);
        free(module_args);
    }
    free(master_args->module_args_list);
    worker_args->module = GONC_LIST_HEAD(module_list);

    if(setgid(master_args->set_gid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    if(setuid(master_args->set_uid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    for(size_t index = 0; index != master_args->worker_count; ++index)
    {
        if(pthread_create(threads + index, &thread_attr, tcpcube_worker_start, worker_args) != 0)
            err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    }

    for(size_t index = 0; index != master_args->worker_count; ++index)
        pthread_join(threads[index], NULL);
    if(tcpcube_module_destroy(GONC_LIST_HEAD(module_list)) == -1)
        warn("%s: %u", __FILE__, __LINE__);
    free(module_list);
    dlclose(dl_handle);
    pthread_mutex_destroy(worker_args->server_socket_mutex);
    free(worker_args->server_socket_mutex);
    close(worker_args->server_socket);
    free(threads);
}
