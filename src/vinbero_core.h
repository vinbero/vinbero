#ifndef _VINBERO_CORE_H
#define _VINBERO_CORE_H

#include <vinbero_com/vinbero_com_Config.h>
#include <vinbero_com/vinbero_com_Module.h>

int
vinbero_core_registerSignalHandlers();
void
vinbero_core_registerExitHandler();
int
vinbero_core_checkConfig(struct vinbero_com_Config* config,
                         const char* moduleId);
int
vinbero_core_initLocalModule(struct vinbero_com_Module* module,
                             struct vinbero_com_Config* config);
int
vinbero_core_loadChildModules(struct vinbero_com_Module* module);
int
vinbero_core_initChildModules(struct vinbero_com_Module* module);
int
vinbero_core_rInitChildModules(struct vinbero_com_Module* module);
int
vinbero_core_start(struct vinbero_com_Module* module);
int
vinbero_core_setGid(struct vinbero_com_Module* module);
int
vinbero_core_setUid(struct vinbero_com_Module* module);

#endif
