#ifndef _VINBERO_CORE_H
#define _VINBERO_CORE_H

#include <stdbool.h>
#include <unistd.h>
#include <jansson.h>
#include "vinbero_Module.h"
#include "vinbero_IBasic.h"

struct vinbero_Core {
    uid_t setUid;
    gid_t setGid;
};

struct vinbero_Core_Interface {
    VINBERO_IBASIC_FUNCTION_POINTERS; 
};

int vinbero_Core_start(struct vinbero_Module* module, struct vinbero_Config* config);

#endif
