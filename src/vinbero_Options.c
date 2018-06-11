#include <getopt.h>
#include <jansson.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <vinbero_common/vinbero_common_Status.h>
#include <vinbero_common/vinbero_common_Error.h>
#include <vinbero_common/vinbero_common_Log.h>
#include "vinbero_Options.h"
#include "vinbero_Help.h"

int vinbero_Options_process(int argc, char* argv[], struct vinbero_common_Config* config) {
    int ret;
    if(argv == NULL || config == NULL)
        return VINBERO_COMMON_ERROR_NULL;
    int loggingFlag = VINBERO_COMMON_LOG_FLAG_ALL & ~VINBERO_COMMON_LOG_FLAG_TRACE;
    const char* configString = NULL;
    const char* configFile = NULL;
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {"logging-flag", required_argument, NULL, 'l'},
        {NULL, 0, NULL, 0}
    };
    json_error_t configError;
    char optionChar;
    bool optionsExist = false;
    while((optionChar = getopt_long(argc, argv, "hi:f:l:", options, NULL)) != (char) - 1) {
        optionsExist = true;
        switch(optionChar) {
        case 'i':
            configString = optarg;
            break;
        case 'f':
            configFile = optarg;
            break;
        case 'l':
            loggingFlag = strtol(optarg, NULL, 10);
            if(loggingFlag == LONG_MIN || loggingFlag == LONG_MAX)
                return VINBERO_COMMON_ERROR_INVALID_OPTION; 
            break;
        case 'h':
            vinbero_Help_print();
            return VINBERO_COMMON_STATUS_EXIT;
        default:
            vinbero_Help_print();
            return VINBERO_COMMON_ERROR_INVALID_OPTION;
        }
    }
    if(!optionsExist) {
        vinbero_Help_print();
        return VINBERO_COMMON_ERROR_INVALID_OPTION;
    }

    vinbero_common_Log_init(loggingFlag);
    vinbero_common_Log_printLogLevelInfo(loggingFlag);

    if(configString != NULL) {
        if((ret = vinbero_common_Config_fromString(config, configString)) < VINBERO_COMMON_STATUS_SUCCESS) {
            return ret;
        }
    } else if(configFile != NULL) {
        if((ret = vinbero_common_Config_fromFile(config, configFile)) < VINBERO_COMMON_STATUS_SUCCESS) {
            return ret;
        }
    } else
         return VINBERO_COMMON_ERROR_INVALID_CONFIG;

    if(config->json == NULL) {
        vinbero_Help_print();
        return VINBERO_COMMON_ERROR_INVALID_CONFIG;
    }

    return VINBERO_COMMON_STATUS_SUCCESS;
}
