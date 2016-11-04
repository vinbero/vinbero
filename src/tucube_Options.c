#include <err.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "tucube_Options.h"
#include "tucube_Core.h"
#include "tucube_Help.h"

int tucube_Options_process(int argc, char* argv[], struct tucube_Core_Config* coreConfig, struct tucube_Module_ConfigList* moduleConfigList) {

    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
    };

    json_t* config = NULL;
    json_error_t configError;
    char optionChar;

    while((optionChar = getopt_long(argc, argv, "hi:f:", options, NULL)) != (char)-1) {
        switch(optionChar) {
        case 'h':
            tucube_Help_print();
            exit(EXIT_SUCCESS);
            break;
        case 'i':
            if(config == NULL) {
                if((config = json_loads(optarg, 0, &configError)) == NULL)
                    errx(EXIT_FAILURE, "%s: %d: %s", configError.source, configError.line, configError.text);
            }
            break;
        case 'f':
            if(config == NULL) {
                if((config = json_load_file(optarg, 0, &configError)) == NULL)
                    errx(EXIT_FAILURE, "%s: %d: %s", configError.source, configError.line, configError.text);
            }
            break;
        }
    }

    if(config == NULL) {
        errx(EXIT_FAILURE, "%s: %u: You need to specify at least one option", __FILE__, __LINE__);
    }

    if(!json_is_array(config))
        errx(EXIT_FAILURE, "%s: %u: config must be an array", __FILE__, __LINE__);

    if(json_array_size(config) < 2)
        errx(EXIT_FAILURE, "%s: %u: config must have at least two elements", __FILE__, __LINE__);
    
    if(!json_is_object(json_array_get(config, 0)))
        errx(EXIT_FAILURE, "%s: %u: first element must be an object", __FILE__, __LINE__);

    coreConfig->json = json_incref(json_array_get(config, 0));

    for(size_t index = 1; index != json_array_size(config); ++index) {
        if(!json_is_array(json_array_get(config, index)))
            errx(EXIT_FAILURE, "%s: %u: elements except first elemnt must be arrays", __FILE__, __LINE__);
        
        struct tucube_Module_Config* moduleConfig = malloc(1 * sizeof(struct tucube_Module_Config));
        GONC_LIST_ELEMENT_INIT(moduleConfig);
        moduleConfig->json = json_incref(json_array_get(config, index));
        GONC_LIST_APPEND(moduleConfigList, moduleConfig);
    }

    if(GONC_LIST_SIZE(moduleConfigList) < 1)
        errx(EXIT_FAILURE, "%s: %u: You need to specify at least one moduleConfig", __FILE__, __LINE__);

    json_decref(config);

    return 0;
}
