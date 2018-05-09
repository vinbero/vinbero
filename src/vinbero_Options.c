#include <err.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include "vinbero_Options.h"
#include "vinbero_Core.h"
#include "vinbero_Error.h"
#include "vinbero_Help.h"
#include <vinbero_log.h>

int vinbero_Options_process(int argc, char* argv[], struct vinbero_Config* config) {
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
    };
    json_error_t configError;
    char optionChar;
    bool optionsExist = false;
    while((optionChar = getopt_long(argc, argv, "hi:f:", options, NULL)) != (char)-1) {
        optionsExist = true;
        switch(optionChar) {
        case 'i':
            if(config->json == NULL) {
                if((config->json = json_loads(optarg, 0, &configError)) == NULL) {
                    VINBERO_LOG_ERROR("%s: %d: %s", configError.source, configError.line, configError.text);
                    return VINBERO_EINVAL;
                }
            }
            break;
        case 'f':
            if(config->json == NULL) {
                if((config->json = json_load_file(optarg, 0, &configError)) == NULL) {
                    VINBERO_LOG_ERROR("%s: %d: %s", configError.source, configError.line, configError.text);
                    return VINBERO_EINVAL;
                }
            }
            break;
        case 'h':
        default:
            vinbero_Help_printAndExit();
            break;
        }
    }

    if(!optionsExist)
        vinbero_Help_printAndExit();

    if(config->json == NULL)
        vinbero_Help_printAndExit();

    return 0;
}
