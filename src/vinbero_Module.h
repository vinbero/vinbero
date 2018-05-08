#ifndef _VINBERO_MODULE_H
#define _VINBERO_MODULE_H

#include <dlfcn.h>
#include <fastdl.h>
#include <jansson.h>
#include <pthread.h>
#include <libgenc/genc_Generic.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_ArrayList.h>
#include "vinbero_Config.h"
#include "vinbero_Error.h"

struct vinbero_Module {
    const char* id;
    const char* name;
    const char* version;
    struct fastdl_Handle dlHandle;
    union genc_Generic localModule;
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    GENC_TREE_NODE(struct vinbero_Module, struct vinbero_Module*);
};

struct vinbero_Module_Ids {
    GENC_ARRAY_LIST(const char*);
};

#define VINBERO_FUNC_ERROR -2
#define VINBERO_FUNC_EXCEPTION -1
#define VINBERO_FUNC_SUCCESS 0
#define VINBERO_FUNC_CONTINUE 1

#define VINBERO_MODULE_DLOPEN(config, module, errorVariable) do { \
    const char* modulePath; \
    if((modulePath = json_string_value(json_object_get(json_object_get((config)->json, (module)->id), "path"))) == NULL) { \
        *(errorVariable) = VINBERO_EINVAL; \
    } else if(fastdl_open(&(module)->dlHandle, modulePath, RTLD_LAZY | RTLD_GLOBAL) == -1) { \
        *(errorVariable) = VINBERO_EUNKNOWN; \
    } else \
        *(errorVariable) = 0; \
} while(0)

#define VINBERO_MODULE_DLSYM(interface, dlHandle, functionName, errorVariable) do { \
    if(fastdl_sym(dlHandle, #functionName, (void**)&(interface)->functionName) == -1) \
        *(errorVariable) = VINBERO_EUNKNOWN; \
    else \
        *(errorVariable) = 0; \
} \
while(0)

#endif
