#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include <string.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_Log.h>
#include "vinbero_core.h"
#include "vinbero_Options.h"

int main(int argc, char* argv[]) {
    int ret;
    struct vinbero_common_Module module;
    memset(&module, 0, sizeof(struct vinbero_common_Module));
    struct vinbero_common_Config config;
    vinbero_common_Config_init(&config);

    if((ret = vinbero_Options_process(argc, argv, &config))) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Options_process(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_checkConfig(&config, "core")) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_checkConfig(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_loadChildModules(&module, NULL, "core", &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_loadChildModules(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_initLocalModule(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_initLocalModule(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_initLocalModule(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_initLocalModule(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_initChildModules(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_initChildModules(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_rInitChildModules(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_rInitChildModules(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_setGid(&module)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_setGid(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_setUid(&module)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_setUid(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_core_registerSignalHandlers()) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_registerSignalHandlers(...) failed");
        return EXIT_FAILURE;
    }
    vinbero_core_registerExitHandler();
    if((ret = vinbero_core_start(&module, &config) < 0)) {
        VINBERO_COMMON_LOG_ERROR("vinbero_core_start(...) failed");
        return EXIT_FAILURE;
    }
    vinbero_common_Config_destroy(&config);

    return 0;
}
