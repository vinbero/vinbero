#include <err.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "tucube_Options.h"
#include "tucube_Core.h"
#include "tucube_Help.h"

bool tucube_Options_checkConfig(struct tucube_Config* config) {
    return true;
}

int tucube_Options_process(int argc, char* argv[], struct tucube_Config* config) {

    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
    };

    json_error_t configError;
    char optionChar;

    while((optionChar = getopt_long(argc, argv, "hi:f:", options, NULL)) != (char)-1) {
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
            tucube_Help_print();
            exit(EXIT_SUCCESS);
            break;
        }
    }

    if(config->json == NULL) {
        tucube_Help_print();
        exit(EXIT_SUCCESS);
    }

    if(tucube_Options_checkConfig(config) == false)
        errx(EXIT_FAILURE, "%s: %u: Invalid config file", __FILE__, __LINE__);

    return 0;
}
