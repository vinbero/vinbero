#include <err.h>
#include <stdlib.h>

#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>

#include "gonm_master.h"
#include "gonm_worker.h"

int main(int argc, char* argv[])
{
    struct gonm_master_args* master_args = malloc(sizeof(struct gonm_master_args));
    master_args->address = "0.0.0.0";
    master_args->port = 8080;
    master_args->backlog = 1024;
    master_args->worker_count = 3;
    master_args->worker_args = malloc(sizeof(struct gonm_worker_args));

    master_args->module_args_list = malloc(sizeof(struct gonm_module_args_list));
    GONC_LIST_INIT(master_args->module_args_list);
    struct gonm_module_args* module_args = malloc(sizeof(struct gonm_module_args));
    GONC_LIST_INIT(module_args);
    char* module_path;
    if((module_path = realpath("libtest.so", NULL)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);
    GONC_STRING_SET(&module_args->module_path, module_path);
    GONC_LIST_APPEND(master_args->module_args_list, module_args);

    gonm_master_start(master_args);
    free(master_args->worker_args);
    free(master_args);
    return 0;
}
