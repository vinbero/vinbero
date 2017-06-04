#include <jansson.h>
#include <libgon_c/gon_c_list.h>
#include "config.h"
#include "tucube_Core.h"
#include "tucube_Options.h"

int main(int argc, char* argv[])
{
    struct tucube_Core core;
    struct tucube_Core_Config coreConfig;
    struct tucube_Module_ConfigList moduleConfigList;
    GON_C_LIST_INIT(&moduleConfigList);
    tucube_Options_process(argc, argv, &coreConfig, &moduleConfigList);
    tucube_Core_start(&core, &coreConfig, &moduleConfigList);
    json_decref(coreConfig.json);
    GON_C_LIST_FOR_EACH(&moduleConfigList, struct tucube_Module_Config, moduleConfig)
        json_decref(moduleConfig->json);

    return 0;
}
