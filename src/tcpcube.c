#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_string.h>
#include "config.h"
#include "tcpcube_master.h"
#include "tcpcube_options.h"
#include "tcpcube_signals.h"

/*void tcpcube_exit_handler()
{
    longjmp()
}*/

int main(int argc, char* argv[])
{
    struct sigaction* signal_action = malloc(sizeof(struct sigaction));
    signal_action->sa_handler = tcpcube_sigint_handler;
    signal_action->sa_flags = SA_RESTART;
    sigfillset(&signal_action->sa_mask);
    sigaction(SIGINT, signal_action, NULL);

    struct tcpcube_master_args* master_args = malloc(sizeof(struct tcpcube_master_args));
    master_args->module_args_list = malloc(sizeof(struct tcpcube_module_args_list));
    GONC_LIST_INIT(master_args->module_args_list);

    master_args->set_uid = geteuid();
    master_args->set_gid = getegid();
    master_args->address = "0.0.0.0";
    master_args->port = 8080;
    master_args->backlog = 1024;
    master_args->worker_count = 4;

    tcpcube_options_process(argc, argv, master_args);

    tcpcube_master_start(master_args);

    free(master_args);
    free(signal_action);

    return 0;
}
