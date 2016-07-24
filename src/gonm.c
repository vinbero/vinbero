#include <err.h>
#include <getopt.h>
#include <stdlib.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>
#include "gonm_help.h"
#include "gonm_master.h"
#include "gonm_worker.h"

int main(int argc, char* argv[])
{    
    struct gonm_master_args* master_args = malloc(sizeof(struct gonm_master_args));
    master_args->worker_args = malloc(sizeof(struct gonm_worker_args));
    master_args->module_args_list = malloc(sizeof(struct gonm_module_args_list));
    GONC_LIST_INIT(master_args->module_args_list);
    struct gonm_module_args* module_args = malloc(sizeof(struct gonm_module_args));
    GONC_LIST_INIT(module_args);

    master_args->address = "0.0.0.0";
    master_args->port = 80;
    master_args->backlog = 1024;
    master_args->worker_count = 4;

    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {"backlog", required_argument, NULL, 'b'},
        {"worker-count", required_argument, NULL, 'w'},
        {"module-args", required_argument, NULL, 'm'},
        {NULL, 0, NULL, 0}
    };

    for(char option_char; option_char != -1; option_char = getopt_long(argc, argv, "ha:p:b:w:m:", options, NULL))
    {
        switch(option_char)
        {
        case 'h':
            gonm_help();
            exit(EXIT_SUCCESS);
            break;
        case 'a':
            master_args->address = optarg;
            break;
        case 'p':
            master_args->port = strtol(optarg, NULL, 10);
            break;
        case 'b':
            master_args->backlog = strtol(optarg, NULL, 10);
            break;
        case 'w':
            master_args->worker_count = strtol(optarg, NULL, 10);
            break;
        case 'm':
            break;
        }
    }

    char* module_path;
    if((module_path = realpath("test_module.so", NULL)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    GONC_STRING_SET(&module_args->module_path, module_path);
    GONC_LIST_APPEND(master_args->module_args_list, module_args);

    gonm_master_start(master_args);
    free(master_args->worker_args);
    free(master_args);
    return 0;
}
