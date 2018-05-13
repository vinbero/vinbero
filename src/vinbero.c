#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include <string.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_Log.h>
#include "vinbero_Core.h"
#include "vinbero_Options.h"

int main(int argc, char* argv[]) {
    int ret;
    struct vinbero_common_Module module;
    memset(&module, 0, sizeof(struct vinbero_common_Module));
    struct vinbero_common_Config config;
    memset(&config, 0, sizeof(struct vinbero_common_Config));
    vinbero_Options_process(argc, argv, &config);

    if((ret = vinbero_Core_checkConfig(&config, "core")) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_checkConfig(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_Core_loadChildModules(&module, NULL, "core", &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_loadChildModules(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_Core_initLocalModule(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_initLocalModule(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_Core_initLocalModule(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_initLocalModule(...) failed");
        return EXIT_FAILURE;
    }

    if((ret = vinbero_Core_initChildModules(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_initChildModules(...) failed");
        return EXIT_FAILURE;
    }
    if((ret = vinbero_Core_rInitChildModules(&module, &config)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_rInitChildModules(...) failed");
        return EXIT_FAILURE;
    }

    if((ret = vinbero_Core_setGid(&module)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_setGid(...) failed");
        return EXIT_FAILURE;
    }

    if((ret = vinbero_Core_setUid(&module)) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_setUid(...) failed");
        return EXIT_FAILURE;
    }

    if((ret = vinbero_Core_registerSignalHandlers()) < 0) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_registerSignalHandlers(...) failed");
        return ret;
    }
    vinbero_Core_registerExitHandler();

    if((ret = vinbero_Core_start(&module, &config) < 0)) {
        VINBERO_COMMON_LOG_ERROR("vinbero_Core_start(...) failed");
        return EXIT_FAILURE;
    }

    vinbero_common_Config_destroy(&config);

    return 0;
}
