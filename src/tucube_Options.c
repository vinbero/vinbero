#include <err.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "tucube_Options.h"
#include "tucube_Master.h"
#include "tucube_Help.h"

void tucube_Options_process(int argc, char* argv[], struct tucube_Master_Args* masterArgs) {
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"set-uid", required_argument, NULL, 'u'},
        {"set-gid", required_argument, NULL, 'g'},
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {"backlog", required_argument, NULL, 'b'},
        {"reuse-port", required_argument, NULL, 'r'},
        {"worker-count", required_argument, NULL, 'w'},
        {"module-args", required_argument, NULL, 'm'},
        {NULL, 0, NULL, 0}
    };

    struct tucube_Module_Args* moduleArgs;
    struct tucube_Module_Arg* moduleArg;
    char* moduleArgsString;
    char* moduleArgsStringTmp;
    char* modulePathString;
    char* moduleArgString;
    char* moduleArgStringTmp;
    char* moduleArgNameOrValue;
    char optionChar;
    while((optionChar = getopt_long(argc, argv, "hu:g:a:p:b:rw:m:", options, NULL)) != (char)-1) {
        switch(optionChar) {
        case 'h':
            tucube_help_print();
            exit(EXIT_SUCCESS);
            break;
        case 'u':
            masterArgs->setUid = strtol(optarg, NULL, 10);
            break;
        case 'g':
            masterArgs->setGid = strtol(optarg, NULL, 10);
            break;
        case 'a':
            masterArgs->address = optarg;
            break;
        case 'p':
            masterArgs->port = strtol(optarg, NULL, 10);
            break;
        case 'b':
            masterArgs->backlog = strtol(optarg, NULL, 10);
            break;
        case 'r':
            masterArgs->reusePort = strtol(optarg, NULL, 10);
            break;
        case 'w':
            masterArgs->workerCount = strtol(optarg, NULL, 10);
            break;
        case 'm':
            moduleArgs = malloc(sizeof(struct tucube_Module_Args));
            GONC_LIST_INIT(moduleArgs);
            GONC_LIST_ELEMENT_INIT(moduleArgs);
            moduleArgsString = strdup(optarg);
            moduleArgsStringTmp = moduleArgsString;
            for(size_t i = 0; (moduleArgString = strsep(&moduleArgsStringTmp, ",")) != NULL; ++i) {
                if(i == 0) {
                    if((modulePathString = realpath(moduleArgString, NULL)) == NULL)
                        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
                    moduleArgs->modulePath = modulePathString;
                    continue;
                }
                moduleArg = malloc(sizeof(struct tucube_Module_Arg));
                GONC_LIST_ELEMENT_INIT(moduleArg);
                GONC_LIST_APPEND(moduleArgs, moduleArg);
                moduleArgString = strdup(moduleArgString);
                moduleArgStringTmp = moduleArgString;
                for(size_t j = 0; j != 2 && (moduleArgNameOrValue = strsep(&moduleArgStringTmp, ":")) != NULL; ++j) {
                    if(j == 0)
                        moduleArg->name = strdup(moduleArgNameOrValue);
                    else
                        moduleArg->value = strdup(moduleArgNameOrValue);
                }
                free(moduleArgString);
            }
            free(moduleArgsString);
            GONC_LIST_APPEND(masterArgs->moduleArgsList, moduleArgs);
            break;
        }
    }
    if(GONC_LIST_SIZE(masterArgs->moduleArgsList) < 1)
        errx(EXIT_FAILURE, "%s: %u: You need at least one --module-args option", __FILE__, __LINE__);
}
