#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include "config.h"
#include "tucube_master.h"
#include "tucube_options.h"

int main(int argc, char* argv[])
{

    struct tucube_master_args* master_args = malloc(1 * sizeof(struct tucube_master_args));
    master_args->module_args_list = malloc(1 * sizeof(struct tucube_module_args_list));
    GONC_LIST_INIT(master_args->module_args_list);

    master_args->set_uid = geteuid();
    master_args->set_gid = getegid();
    master_args->address = "0.0.0.0";
    master_args->port = 8080;
    master_args->backlog = 1024;
    master_args->reuse_port = 0;
    master_args->worker_count = 4;

    tucube_options_process(argc, argv, master_args);

    tucube_master_init_core(master_args);

    if(setgid(master_args->set_gid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setuid(master_args->set_uid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    tucube_master_init_modules(master_args);

    tucube_master_start(master_args);

    free(master_args);

    return 0;
}
