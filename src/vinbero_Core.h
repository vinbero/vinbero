#ifndef _VINBERO_CORE_H
#define _VINBERO_CORE_H

#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_Config.h>

int vinbero_Core_registerSignalHandlers();
void vinbero_Core_registerExitHandler();
int vinbero_Core_checkConfig(struct vinbero_common_Config* config, const char* moduleId);
int vinbero_Core_initLocalModule(struct vinbero_common_Module* module, struct vinbero_common_Config* config);
int vinbero_Core_start(struct vinbero_common_Module* module, struct vinbero_common_Config* config);
int vinbero_Core_loadChildModules(struct vinbero_common_Module* module, struct vinbero_common_Module* parentModule, const char* moduleId, struct vinbero_common_Config* config);
int vinbero_Core_initChildModules(struct vinbero_common_Module* module, struct vinbero_common_Config* config);
int vinbero_Core_rInitChildModules(struct vinbero_common_Module* module, struct vinbero_common_Config* config);
int vinbero_Core_destroyChildModules(struct vinbero_common_Module* module);
int vinbero_Core_rDestroyChildModules(struct vinbero_common_Module* module);
int vinbero_Core_setGid(struct vinbero_common_Module* module);
int vinbero_Core_setUid(struct vinbero_common_Module* module);


#endif
