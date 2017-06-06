#ifndef _TUCUBE_IBASE_H
#define _TUCUBE_IBASE_H

#include "tucube_Module.h"

#define TUCUBE_IBASE_FUNCTIONS                                                                           \
int tucube_IBase_init(struct tucube_Module_Config* moduleConfig, struct tucube_Module_List* moduleList); \
int tucube_IBase_tlInit(struct tucube_Module* module, struct tucube_Module_Config* moduleConfig);        \
int tucube_IBase_tlDestroy(struct tucube_Module* module);                                                \
int tucube_IBase_destroy(struct tucube_Module* module)

#define TUCUBE_IBASE_FUNCTION_POINTERS                                              \
int (*tucube_IBase_init)(struct tucube_Module_Config*, struct tucube_Module_List*); \
int (*tucube_IBase_destroy)(struct tucube_Module*);                                 \
int (*tucube_IBase_tlInit)(struct tucube_Module*, struct tucube_Module_Config*);    \
int (*tucube_IBase_tlDestroy)(struct tucube_Module*)

#endif
