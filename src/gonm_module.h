#ifndef _GONM_MODULE_H
#define _GONM_MODULE_H

#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>

struct gonm_module
{
    void* object;
    size_t object_size;
    GONC_LIST_ELEMENT(struct gonm_module);
};

struct gonm_module_list
{
    GONC_LIST_ELEMENT(struct gonm_module);
};

struct module_arg
{
    struct gonc_string name;
    struct gonc_string value;
    GONC_LIST_ELEMENT(struct module_arg);
};

struct module_args
{
    GONC_LIST_ELEMENT(struct module_args);
    GONC_LIST(struct module_arg);
};

struct module_args_list
{
    GONC_LIST(struct module_args);
};

#endif
