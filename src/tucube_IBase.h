#ifndef _TUCUBE_IBASE_H
#define _TUCUBE_IBASE_H

#include "tucube_Module.h"

#define TUCUBE_IBASE_FUNCTIONS                                                                                                 \
int tucube_IBase_init(struct tucube_Config* config, struct tucube_Module* parentModule, const char* moduleName, void* args[]); \
int tucube_IBase_tlInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]);                             \
int tucube_IBase_tlDestroy(struct tucube_Module* module);                                                                      \
int tucube_IBase_destroy(struct tucube_Module* module)

#endif
