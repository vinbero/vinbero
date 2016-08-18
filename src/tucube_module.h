#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <pthread.h>
#include <libgonc/gonc_list.h>

struct tucube_module
{
    void* dl_handle;
    union
    {
        int integer;
        unsigned int uinteger;
        void* pointer;
    };
    pthread_rwlock_t* rwlock;
    pthread_key_t* tlmodule_key;
    GONC_LIST_ELEMENT(struct tucube_module);
};

struct tucube_module_list
{
    GONC_LIST(struct tucube_module);
};

struct tucube_module_arg
{
    char* name;
    char* value;
    GONC_LIST_ELEMENT(struct tucube_module_arg);
};

struct tucube_module_args
{
    char* module_path;
    GONC_LIST_ELEMENT(struct tucube_module_args);
    GONC_LIST(struct tucube_module_arg);
};

struct tucube_module_args_list
{
    GONC_LIST(struct tucube_module_args);
};

#endif
