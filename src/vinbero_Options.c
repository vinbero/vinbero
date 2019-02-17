#include <getopt.h>
#include <jansson.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <vinbero_com/vinbero_com_Status.h>
#include <vinbero_com/vinbero_com_Error.h>
#include <vinbero_com/vinbero_com_Log.h>
#include "vinbero_Options.h"
#include "vinbero_Help.h"
#include "config.h"

int vinbero_Options_process(int argc, char* argv[], struct vinbero_com_Config* config) {
    int ret;
    if(argv == NULL || config == NULL)
        return VINBERO_COM_ERROR_NULL;
    int loggingFlag = VINBERO_COM_LOG_FLAG_ALL & ~VINBERO_COM_LOG_FLAG_TRACE;
    int loggingOption = VINBERO_COM_LOG_OPTION_COLOR;

    const char* configString = NULL;
    const char* configFile = NULL;
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'c'},
        {"logging-flag", required_argument, NULL, 'f'},
        {"logging-option", required_argument, NULL, 'o'},
        {NULL, 0, NULL, 0}
    };
    char optionChar;
    bool optionsExist = false;
    while((optionChar = getopt_long(argc, argv, "i:c:f:o:vh", options, NULL)) != (char) - 1) {
        optionsExist = true;
        switch(optionChar) {
        case 'i':
            configString = optarg;
            break;
        case 'c':
            configFile = optarg;
            break;
        case 'f':
            loggingFlag = strtol(optarg, NULL, 10);
            if(loggingFlag == LONG_MIN || loggingFlag == LONG_MAX)
                return VINBERO_COM_ERROR_INVALID_OPTION; 
            break;
        case 'o':
            loggingOption = strtol(optarg, NULL, 10);
            if(loggingOption == LONG_MIN || loggingOption == LONG_MAX)
                return VINBERO_COM_ERROR_INVALID_OPTION; 
            break;
        case 'v':
            printf("%s\n", VINBERO_VERSION);
            return VINBERO_COM_STATUS_EXIT;
        case 'h':
            vinbero_Help_print();
            return VINBERO_COM_STATUS_EXIT;
        default:
            vinbero_Help_print();
            return VINBERO_COM_ERROR_INVALID_OPTION;
        }
    }
    if(!optionsExist) {
        vinbero_Help_print();
        return VINBERO_COM_ERROR_INVALID_OPTION;
    }

    vinbero_com_Log_init(loggingFlag, loggingOption);
    vinbero_com_Log_printLogLevelInfo(loggingFlag);

    if(configString != NULL) {
        if((ret = vinbero_com_Config_fromString(config, configString)) < VINBERO_COM_STATUS_SUCCESS) {
            return ret;
        }
    } else if(configFile != NULL) {
        if((ret = vinbero_com_Config_fromFile(config, configFile)) < VINBERO_COM_STATUS_SUCCESS) {
            return ret;
        }
    } else
         return VINBERO_COM_ERROR_INVALID_CONFIG;

    if(config->json == NULL) {
        vinbero_Help_print();
        return VINBERO_COM_ERROR_INVALID_CONFIG;
    }

    return VINBERO_COM_STATUS_SUCCESS;
}
