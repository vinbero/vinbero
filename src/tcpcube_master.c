#include <arpa/inet.h>
#include <dlfcn.h>
#include <err.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libgonc/gonc_list.h>
#include "config.h"
#include "tcpcube_master.h"
#include "tcpcube_worker.h"

static pthread_key_t tcpcube_master_pthread_key;

static void tcpcube_master_exit_handler()
{
    longjmp(pthread_getspecific(tcpcube_master_pthread_key), 1);
}

void tcpcube_master_init_core(struct tcpcube_master_args* master_args)
{
    struct sockaddr_in server_address_sockaddr_in;
    memset(server_address_sockaddr_in.sin_zero, 0, sizeof(server_address_sockaddr_in.sin_zero));
    server_address_sockaddr_in.sin_family = AF_INET;
    inet_aton(master_args->address, &server_address_sockaddr_in.sin_addr);
    server_address_sockaddr_in.sin_port = htons(master_args->port);

    master_args->worker_args = malloc(sizeof(struct tcpcube_worker_args));

    if((master_args->worker_args->server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(master_args->worker_args->server_socket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(master_args->worker_args->server_socket, SOL_SOCKET, SO_REUSEPORT, &master_args->reuse_port, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(bind(master_args->worker_args->server_socket, (struct sockaddr*)&server_address_sockaddr_in, sizeof(struct sockaddr)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(listen(master_args->worker_args->server_socket, master_args->backlog) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    master_args->worker_args->server_socket_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(master_args->worker_args->server_socket_mutex, NULL);
}

void tcpcube_master_init_modules(struct tcpcube_master_args* master_args)
{
    if((master_args->dl_handle = dlopen(GONC_LIST_HEAD(master_args->module_args_list)->module_path.chars, RTLD_LAZY)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((master_args->tcpcube_module_init = dlsym(master_args->dl_handle, "tcpcube_module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tcpcube_module_init", __FILE__, __LINE__);

    if((master_args->worker_args->tcpcube_module_start = dlsym(master_args->dl_handle, "tcpcube_module_start")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tcpcube_module_start", __FILE__, __LINE__);

    if((master_args->tcpcube_module_destroy = dlsym(master_args->dl_handle, "tcpcube_module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tcpcube_module_destroy", __FILE__, __LINE__);

    master_args->module_list = malloc(sizeof(struct tcpcube_module_list));
    GONC_LIST_INIT(master_args->module_list);

    if(master_args->tcpcube_module_init(GONC_LIST_HEAD(master_args->module_args_list), master_args->module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tcpcube_module_init() failed", __FILE__, __LINE__);

    master_args->worker_args->module = GONC_LIST_HEAD(master_args->module_list);

    GONC_LIST_REMOVE_FOR_EACH(master_args->module_args_list, struct tcpcube_module_args, module_args)
    {
        GONC_LIST_REMOVE_FOR_EACH(module_args, struct tcpcube_module_arg, module_arg)
        {
            GONC_LIST_REMOVE(module_args, module_arg);
            free(module_arg->name.chars);
            free(module_arg->value.chars);
            free(module_arg);
        }
        GONC_LIST_REMOVE(master_args->module_args_list, module_args);
        free(module_args->module_path.chars);
        free(module_args);
    }
    free(master_args->module_args_list);
}

void tcpcube_master_start(struct tcpcube_master_args* master_args)
{
    pthread_t* worker_threads;
    pthread_attr_t worker_thread_attr;
    jmp_buf* tcpcube_master_jmp_buf = malloc(sizeof(jmp_buf));
    if(setjmp(*tcpcube_master_jmp_buf) == 0)
    {
        pthread_key_create(&tcpcube_master_pthread_key, NULL);
        pthread_setspecific(tcpcube_master_pthread_key, tcpcube_master_jmp_buf);
        atexit(tcpcube_master_exit_handler);

        worker_threads = malloc(master_args->worker_count * sizeof(pthread_t));
        
        pthread_attr_init(&worker_thread_attr);
        pthread_attr_setdetachstate(&worker_thread_attr, PTHREAD_CREATE_JOINABLE);

        for(size_t index = 0; index != master_args->worker_count; ++index)
        {
            if(pthread_create(worker_threads + index, &worker_thread_attr, tcpcube_worker_start, master_args->worker_args) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }
        pause();
    }
    free(tcpcube_master_jmp_buf);
    pthread_key_delete(tcpcube_master_pthread_key);

    close(master_args->worker_args->server_socket);

    pthread_mutex_destroy(master_args->worker_args->server_socket_mutex);
    free(master_args->worker_args->server_socket_mutex);
    free(master_args->worker_args);

    for(size_t index = 0; index != master_args->worker_count; ++index)
        pthread_detach(worker_threads[index]);

    pthread_attr_destroy(&worker_thread_attr);
    free(worker_threads);

    if(master_args->tcpcube_module_destroy(GONC_LIST_HEAD(master_args->module_list)) == -1)
        warn("%s: %u", __FILE__, __LINE__);
    free(master_args->module_list);
    dlclose(master_args->dl_handle);
}
