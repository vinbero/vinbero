#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <pthread.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>

struct tucube_module
{
    void* object;
    size_t object_size;
    pthread_rwlock_t* object_rwlock;
    pthread_key_t* tlmodule_key;
    GONC_LIST_ELEMENT(struct tucube_module);
};

struct tucube_module_list
{
    GONC_LIST(struct tucube_module);
};

struct tucube_module_arg
{
    struct gonc_string name;
    struct gonc_string value;
    GONC_LIST_ELEMENT(struct tucube_module_arg);
};

struct tucube_module_args
{
    struct gonc_string module_path;
    GONC_LIST_ELEMENT(struct tucube_module_args);
    GONC_LIST(struct tucube_module_arg);
};

struct tucube_module_args_list
{
    GONC_LIST(struct tucube_module_args);
};

#endif
