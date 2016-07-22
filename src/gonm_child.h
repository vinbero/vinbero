#ifndef _GONM_CHILD_H
#define _GONM_CHILD_H

struct gonm_child_args
{
    char* address;
    int port;
    int backlog;
};

void* gonm_child_start(void* child_args);

#endif
