#include <err.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "tucube_Options.h"
#include "tucube_Master.h"
#include "tucube_Help.h"

int tucube_Options_process(int argc, char* argv[], struct tucube_Core_Config* coreConfig, struct tucube_Module_ConfigList* moduleConfigList) {

    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"config-inline", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
    };

    json_t* configRoot;
    char optionChar;

    while((optionChar = getopt_long(argc, argv, "hi:f:", options, NULL)) != (char)-1) {
        switch(optionChar) {
        case 'h':
            tucube_Help_print();
            exit(EXIT_SUCCESS);
            break;
        case 'i':
            configRoot = json_loads(optarg);
            break;
        case 'f':
            configRoot = json_load_file(optarg);
            break;
        }
    }

    if(configRoot == NULL)
        errx(EXIT_FAILURE, "%s: %u: %s", error.source, error.line, error.text);

    if(!json_is_array(configRoot))
        errx(EXIT_FAILURE, "%s: %u: configRoot must be an array", __FILE__, __LINE__);

    if(json_array_size(configRoot) < 2)
        errx(EXIT_FAILURE, "%s: %u: configRoot must have at least two elements", __FILE__, __LINE__);
    
    if(!json_is_object(json_array_get(configRoot, 0)))
        errx(EXIT_FAILURE, "%s: %u: first element must be an object", __FILE__, __LINE__);

    coreConfig->json = json_incref(json_array_get(configRoot, 0));

    for(size_t index = 1; index != json_array_size(configRoot); ++index)
    {
        if(!json_is_array(json_array_get(configRoot, index)))
            errx(EXIT_FAILURE, "%s: %u: elements except first elemnt must be arrays", __FILE__, __LINE__);
        
        struct tucube_Module_Config* moduleConfig = malloc(sizeof(struct tucube_Module_Config));
        GONC_LIST_ELEMENT_INIT(moduleConfig);
        moduleConfig->json = json_incref(json_array_get(configRoot, index));
        GONC_LIST_APPEND(moduleConfigList, moduleConfig);
    }

    if(GONC_LIST_SIZE(moduleConfigList) < 1)
        errx(EXIT_FAILURE, "%s: %u: You need to specify at least one moduleConfig", __FILE__, __LINE__);

    json_decref(configRoot);

    return 0;
}
