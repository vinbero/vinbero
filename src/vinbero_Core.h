#ifndef _VINBERO_CORE_H
#define _VINBERO_CORE_H

#include "vinbero_Module.h"
#include "vinbero_Config.h"

int vinbero_Core_registerSignalHandlers();
void vinbero_Core_registerExitHandler();
int vinbero_Core_checkConfig(struct vinbero_Config* config, const char* moduleId);
int vinbero_Core_initLocalModule(struct vinbero_Module* module, struct vinbero_Config* config);
int vinbero_Core_start(struct vinbero_Module* module, struct vinbero_Config* config);
int vinbero_Core_loadChildModules(struct vinbero_Module* module, struct vinbero_Module* parentModule, const char* moduleId, struct vinbero_Config* config);
int vinbero_Core_initChildModules(struct vinbero_Module* module, struct vinbero_Config* config);
int vinbero_Core_rInitChildModules(struct vinbero_Module* module, struct vinbero_Config* config);
int vinbero_Core_destroyChildModules(struct vinbero_Module* module);
int vinbero_Core_rDestroyChildModules(struct vinbero_Module* module);
int vinbero_Core_setGid(struct vinbero_Module* module);
int vinbero_Core_setUid(struct vinbero_Module* module);


#endif
