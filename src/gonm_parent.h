#ifndef _GONM_PARENT_H
#define _GONM_PARENT_H

#include "gonm_child.h"

struct gonm_parent_args
{
    char* address;
    int port;
    int backlog;
    size_t child_count;
    struct gonm_child_args* child_args;
};

void gonm_parent_start(struct gonm_parent_args* parent_args);

#endif
