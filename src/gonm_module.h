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
    GONC_LIST(struct gonm_module);
};

struct gonm_module_arg
{
    struct gonc_string name;
    struct gonc_string value;
    GONC_LIST_ELEMENT(struct gonm_module_arg);
};

struct gonm_module_args
{
    struct gonc_string module_path;
    GONC_LIST_ELEMENT(struct gonm_module_args);
    GONC_LIST(struct gonm_module_arg);
};

struct gonm_module_args_list
{
    GONC_LIST(struct gonm_module_args);
};

#endif
