#include <getopt.h>
#include <jansson.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <vinbero_common/vinbero_common_Error.h>
#include <vinbero_common/vinbero_common_Log.h>
#include "vinbero_Options.h"
#include "vinbero_Help.h"

static void printLogFlagInfo(int flag) {
    if(flag & VINBERO_COMMON_LOG_FLAG_TRACE)
        VINBERO_COMMON_LOG_INFO("TRACE LEVEL LOGGING ENABLED");
    if(flag & VINBERO_COMMON_LOG_FLAG_DEBUG)
        VINBERO_COMMON_LOG_INFO("DEBUG LEVEL LOGGING ENABLED");
    if(flag & VINBERO_COMMON_LOG_FLAG_INFO)
        VINBERO_COMMON_LOG_INFO("INFO LEVEL LOGGING ENABLED");
    if(flag & VINBERO_COMMON_LOG_FLAG_WARN)
        VINBERO_COMMON_LOG_INFO("WARN LEVEL LOGGING ENABLED");
    if(flag & VINBERO_COMMON_LOG_FLAG_ERROR)
        VINBERO_COMMON_LOG_INFO("ERROR LEVEL LOGGING ENABLED");
    if(flag & VINBERO_COMMON_LOG_FLAG_FATAL)
        VINBERO_COMMON_LOG_INFO("FATAL LEVEL LOGGING ENABLED");
}

int vinbero_Options_process(int argc, char* argv[], struct vinbero_common_Config* config) {
    int ret;
    vinbero_common_Log_flag = VINBERO_COMMON_LOG_FLAG_ALL & ~VINBERO_COMMON_LOG_FLAG_TRACE;
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
    while((optionChar = getopt_long(argc, argv, "hi:f:l:", options, NULL)) != (char)-1) {
        optionsExist = true;
        switch(optionChar) {
        case 'i':
            configString = optarg;
            break;
        case 'f':
            configFile = optarg;
            break;
        case 'l':
            vinbero_common_Log_flag = strtol(optarg, NULL, 10);
            if(vinbero_common_Log_flag == LONG_MIN || vinbero_common_Log_flag == LONG_MAX)
                return -errno;
            printLogFlagInfo(vinbero_common_Log_flag);
            break;
        case 'h':
        default:
            vinbero_Help_printAndExit();
            break;
        }
    }

    if(!optionsExist)
        vinbero_Help_printAndExit();

    if(configString != NULL) {
        if((ret = vinbero_common_Config_fromString(config, configString)) < 0) {
            return ret;
        }
    } else if(configFile != NULL) {
        if((ret = vinbero_common_Config_fromFile(config, configFile)) < 0) {
            return ret;
        }
    }

    if(config->json == NULL)
        vinbero_Help_printAndExit();

    return 0;
}
