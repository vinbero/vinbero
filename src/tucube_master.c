#include <arpa/inet.h>
#include <dlfcn.h>
#include <err.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_uinttostr.h>
#include "config.h"
#include "tucube_master.h"
#include "tucube_worker.h"

static pthread_key_t tucube_master_tlkey;

static void tucube_master_sigint_handler(int signal_number)
{
    exit(EXIT_FAILURE);
}

static void tucube_master_exit_handler()
{
    if(syscall(SYS_gettid) == getpid())
    {
        jmp_buf* jump_buffer = pthread_getspecific(tucube_master_tlkey);
        if(jump_buffer != NULL)
            longjmp(*jump_buffer, 1);
    }
}

static void tucube_master_register_signal_handlers()
{
    struct sigaction signal_action;
    signal_action.sa_handler = tucube_master_sigint_handler;
    signal_action.sa_flags = SA_RESTART;
    if(sigfillset(&signal_action.sa_mask) == -1)
        err(EXIT_FAILURE, "%s, %u", __FILE__, __LINE__);
    if(sigaction(SIGINT, &signal_action, NULL) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    signal_action.sa_handler = SIG_IGN;
    signal_action.sa_flags = SA_RESTART;
    if(sigfillset(&signal_action.sa_mask) == -1)
        err(EXIT_FAILURE, "%s, %u", __FILE__, __LINE__);
    if(sigaction(SIGPIPE, &signal_action, NULL) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
}

void tucube_master_init_core(struct tucube_master_args* master_args)
{
    master_args->worker_args = malloc(1 * sizeof(struct tucube_worker_args));

    master_args->worker_args->exit = false;
    master_args->worker_args->exit_mutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(master_args->worker_args->exit_mutex, NULL);
    master_args->worker_args->exit_cond = malloc(1 * sizeof(pthread_cond_t));
    pthread_cond_init(master_args->worker_args->exit_cond, NULL);

    struct sockaddr_in server_address_sockaddr_in;
    memset(server_address_sockaddr_in.sin_zero, 0, 1 * sizeof(server_address_sockaddr_in.sin_zero));
    server_address_sockaddr_in.sin_family = AF_INET;
    inet_aton(master_args->address, &server_address_sockaddr_in.sin_addr);
    server_address_sockaddr_in.sin_port = htons(master_args->port);

    if((master_args->worker_args->server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(master_args->worker_args->server_socket, SOL_SOCKET, SO_REUSEADDR, &(const int){1}, sizeof(int)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setsockopt(master_args->worker_args->server_socket, SOL_SOCKET, SO_REUSEPORT, &master_args->reuse_port, sizeof(int)) == -1)
        warn("%s: %u", __FILE__, __LINE__);

    if(bind(master_args->worker_args->server_socket, (struct sockaddr*)&server_address_sockaddr_in, sizeof(struct sockaddr)) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(listen(master_args->worker_args->server_socket, master_args->backlog) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    master_args->worker_args->server_socket_mutex = malloc(1 * sizeof(pthread_mutex_t));
    pthread_mutex_init(master_args->worker_args->server_socket_mutex, NULL);
}

void tucube_master_init_modules(struct tucube_master_args* master_args)
{
    if((master_args->dl_handle = dlopen(GONC_LIST_HEAD(master_args->module_args_list)->module_path, RTLD_LAZY)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((master_args->tucube_module_init = dlsym(master_args->dl_handle, "tucube_module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_module_init()", __FILE__, __LINE__);

    if((master_args->worker_args->tucube_module_tlinit = dlsym(master_args->dl_handle, "tucube_module_tlinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_module_tlinit()", __FILE__, __LINE__);

    if((master_args->worker_args->tucube_module_start = dlsym(master_args->dl_handle, "tucube_module_start")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_module_start()", __FILE__, __LINE__);

    if((master_args->worker_args->tucube_module_tldestroy = dlsym(master_args->dl_handle, "tucube_module_tldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_module_tldestroy()", __FILE__, __LINE__);

    if((master_args->tucube_module_destroy = dlsym(master_args->dl_handle, "tucube_module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_module_destroy()", __FILE__, __LINE__);

    GONC_LIST_FOR_EACH(master_args->module_args_list, struct tucube_module_args, module_args)
    {
        struct tucube_module_arg* module_arg = malloc(1 * sizeof(struct tucube_module_arg));
        GONC_LIST_ELEMENT_INIT(module_arg);
        module_arg->name = strdup("tucube-worker-count");
        gonc_uinttostr(master_args->worker_count, 10, &module_arg->value);
        GONC_LIST_APPEND(module_args, module_arg);
    }

    master_args->module_list = malloc(1 * sizeof(struct tucube_module_list));
    GONC_LIST_INIT(master_args->module_list);
    if(master_args->tucube_module_init(GONC_LIST_HEAD(master_args->module_args_list), master_args->module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_module_init() failed", __FILE__, __LINE__);

    master_args->worker_args->module = GONC_LIST_HEAD(master_args->module_list);
    master_args->worker_args->module_args = GONC_LIST_HEAD(master_args->module_args_list);
}

void tucube_master_start(struct tucube_master_args* master_args)
{
    tucube_master_register_signal_handlers();
    pthread_t* worker_threads = NULL;
    pthread_attr_t worker_thread_attr;
    jmp_buf* jump_buffer = malloc(1 * sizeof(jmp_buf));
    if(setjmp(*jump_buffer) == 0)
    {
        pthread_key_create(&tucube_master_tlkey, NULL);
        pthread_setspecific(tucube_master_tlkey, jump_buffer);

        pthread_attr_init(&worker_thread_attr);
        pthread_attr_setdetachstate(&worker_thread_attr, PTHREAD_CREATE_JOINABLE);

        worker_threads = malloc(master_args->worker_count * sizeof(pthread_t));

        atexit(tucube_master_exit_handler);

        for(size_t index = 0; index != master_args->worker_count; ++index)
        {
            if(pthread_create(worker_threads + index, &worker_thread_attr, tucube_worker_start, master_args->worker_args) != 0)
                err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
        }

        pthread_mutex_lock(master_args->worker_args->exit_mutex);
        while(master_args->worker_args->exit != true)
        {
            pthread_cond_wait(master_args->worker_args->exit_cond,
                 master_args->worker_args->exit_mutex);
        }
        pthread_mutex_unlock(master_args->worker_args->exit_mutex);

        for(size_t index = 0; index != master_args->worker_count; ++index)
        {
            pthread_cancel(worker_threads[index]);
            pthread_join(worker_threads[index], NULL);
        }
    }
    free(jump_buffer);
    pthread_key_delete(tucube_master_tlkey);

    pthread_cond_destroy(master_args->worker_args->exit_cond);
    free(master_args->worker_args->exit_cond);
    pthread_mutex_destroy(master_args->worker_args->exit_mutex);
    free(master_args->worker_args->exit_mutex);

    GONC_LIST_REMOVE_FOR_EACH(master_args->module_args_list, struct tucube_module_args, module_args)
    {
        GONC_LIST_REMOVE_FOR_EACH(module_args, struct tucube_module_arg, module_arg)
        {
            GONC_LIST_REMOVE(module_args, module_arg);
            free(module_arg->name);
            free(module_arg->value);
            free(module_arg);
        }
        GONC_LIST_REMOVE(master_args->module_args_list, module_args);
        free(module_args->module_path);
        free(module_args);
    }
    free(master_args->module_args_list);
/*
    for(size_t index = 0; index != master_args->worker_count; ++index)
    {
        pthread_cancel(worker_threads[index]);
        pthread_join(worker_threads[index], NULL);
    }
*/
    close(master_args->worker_args->server_socket);
    pthread_mutex_destroy(master_args->worker_args->server_socket_mutex);
    free(master_args->worker_args->server_socket_mutex);
    free(master_args->worker_args);

    pthread_attr_destroy(&worker_thread_attr);
    free(worker_threads);

    if(master_args->tucube_module_destroy(GONC_LIST_HEAD(master_args->module_list)) == -1)
        warn("%s: %u", __FILE__, __LINE__);
    free(master_args->module_list);
//    dlclose(master_args->dl_handle);
}
