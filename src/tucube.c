#include <jansson.h>
#include <libgenc/genc_list.h>
#include "tucube_Core.h"
#include "tucube_Options.h"

int main(int argc, char* argv[])
{
    struct tucube_Core core;
    struct tucube_Core_Config coreConfig;
    struct tucube_Module_ConfigList moduleConfigList;
    GENC_LIST_INIT(&moduleConfigList);
    tucube_Options_process(argc, argv, &coreConfig, &moduleConfigList);
    tucube_Core_start(&core, &coreConfig, &moduleConfigList);
    json_decref(coreConfig.json);
    GENC_LIST_FOR_EACH(&moduleConfigList, struct tucube_Module_Config, moduleConfig)
        json_decref(moduleConfig->json);

    return 0;
}
