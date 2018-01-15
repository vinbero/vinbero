#include <err.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include "vinbero_Options.h"
#include "vinbero_Core.h"
#include "vinbero_Help.h"

bool vinbero_Options_checkConfig(struct vinbero_Config* config) {
    return true;
}

int vinbero_Options_process(int argc, char* argv[], struct vinbero_Config* config) {

    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
    };

    json_error_t configError;
    char optionChar;
    bool isThereAnyOptions = false;

    while((optionChar = getopt_long(argc, argv, "hi:f:", options, NULL)) != (char)-1) {
        isThereAnyOptions = true;
        switch(optionChar) {
        case 'i':
            if(config->json == NULL) {
                if((config->json = json_loads(optarg, 0, &configError)) == NULL)
                    errx(EXIT_FAILURE, "%s: %d: %s", configError.source, configError.line, configError.text);
            }
            break;
        case 'f':
            if(config->json == NULL) {
                if((config->json = json_load_file(optarg, 0, &configError)) == NULL)
                    errx(EXIT_FAILURE, "%s: %d: %s", configError.source, configError.line, configError.text);
            }
            break;
        case 'h':
        default:
            vinbero_Help_printAndExit();
            break;
        }
    }

    if(!isThereAnyOptions)
        vinbero_Help_printAndExit();

    if(config->json == NULL)
        vinbero_Help_printAndExit();

    if(vinbero_Options_checkConfig(config) == false)
        errx(EXIT_FAILURE, "%s: %u: Invalid config file", __FILE__, __LINE__);

    return 0;
}
