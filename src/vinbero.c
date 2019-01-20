#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include <string.h>
#include <vinbero_common/vinbero_common_Status.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_Log.h>
#include "vinbero_core.h"
#include "vinbero_Options.h"

int main(int argc, char* argv[]) {
    int ret;

    struct vinbero_common_Config config;
    vinbero_common_Config_init(&config);

    if((ret = vinbero_Options_process(argc, argv, &config)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Options_process() failed");
        return EXIT_FAILURE;
    } else if(ret == VINBERO_COMMON_STATUS_EXIT)
        return EXIT_SUCCESS;

    struct vinbero_common_Module module;
    GENC_TREE_NODE_INIT(&module);

    if((ret = vinbero_core_initLocalModule(&module, &config)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_initLocalModule() failed");
        return EXIT_FAILURE;
    }

    if((ret = vinbero_core_checkConfig(module.config, module.id)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_checkConfig() failed");
        return EXIT_FAILURE;
    }

    if((ret = vinbero_core_loadChildModules(&module)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_loadChildModules() failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_initChildModules(&module)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_initChildModules() failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_rInitChildModules(&module)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_rInitChildModules() failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_setGid(&module)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_setGid() failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_setUid(&module)) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_setUid() failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_registerSignalHandlers()) < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_registerSignalHandlers() failed");
        return EXIT_FAILURE;
    }
    vinbero_core_registerExitHandler();
    if((ret = vinbero_core_start(&module) < VINBERO_COMMON_STATUS_SUCCESS)) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_start() failed");
        return EXIT_FAILURE;
    }
    vinbero_common_Config_destroy(&config);

    return 0;
}
