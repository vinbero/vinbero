#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>
#include "gonm_master.h"
#include "gonm_options.h"

int main(int argc, char* argv[])
{    
    struct gonm_master_args* master_args = malloc(sizeof(struct gonm_master_args));
    master_args->module_args_list = malloc(sizeof(struct gonm_module_args_list));
    GONC_LIST_INIT(master_args->module_args_list);

    master_args->set_uid = geteuid();
    master_args->address = "0.0.0.0";
    master_args->port = 8080;
    master_args->backlog = 1024;
    master_args->worker_count = 4;

    gonm_options_process(argc, argv, master_args);

    gonm_master_start(master_args);

    free(master_args);

    return 0;
}
