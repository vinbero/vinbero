#ifndef _TUCUBE_CORE_H
#define _TUCUBE_CORE_H

#include <stdbool.h>
#include <unistd.h>
#include <jansson.h>
#include "tucube_Module.h"
#include "tucube_ICore.h"

struct tucube_Core {
    uid_t setUid;
    gid_t setGid;
};

struct tucube_Core_Interface {
    TUCUBE_ICORE_FUNCTION_POINTERS; 
};

int tucube_Core_start(struct tucube_Module* module, struct tucube_Config* config);

#endif
