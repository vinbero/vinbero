#ifndef _TCPCUBE_MODULE_H
#define _TCPCUBE_MODULE_H

#include <pthread.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>

struct tcpcube_module
{
    void* object;
    size_t object_size;
    pthread_mutex_t* object_mutex;
    GONC_LIST_ELEMENT(struct tcpcube_module);
};

#define TCPCUBE_MODULE_CAST(module, type) \
((type)module)

struct tcpcube_module_list
{
    GONC_LIST(struct tcpcube_module);
};

struct tcpcube_module_arg
{
    struct gonc_string name;
    struct gonc_string value;
    GONC_LIST_ELEMENT(struct tcpcube_module_arg);
};

struct tcpcube_module_args
{
    struct gonc_string module_path;
    GONC_LIST_ELEMENT(struct tcpcube_module_args);
    GONC_LIST(struct tcpcube_module_arg);
};

struct tcpcube_module_args_list
{
    GONC_LIST(struct tcpcube_module_args);
};

#endif
