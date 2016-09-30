#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include "config.h"
#include "tucube_Master.h"
#include "tucube_Options.h"

int main(int argc, char* argv[])
{
    struct tucube_Master_Args* masterArgs = malloc(1 * sizeof(struct tucube_Master_Args));
    masterArgs->moduleArgsList = malloc(1 * sizeof(struct tucube_Module_ArgsList));
    GONC_LIST_INIT(masterArgs->moduleArgsList);

    masterArgs->setUid = geteuid();
    masterArgs->setGid = getegid();
    masterArgs->address = "0.0.0.0";
    masterArgs->port = 8080;
    masterArgs->backlog = 1024;
    masterArgs->reusePort = 0;
    masterArgs->workerCount = 4;

    tucube_Options_process(argc, argv, masterArgs);

    tucube_Master_initCore(masterArgs);

    if(setgid(masterArgs->setGid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setuid(masterArgs->setUid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    tucube_Master_initModules(masterArgs);

    tucube_Master_start(masterArgs);

    free(masterArgs);
    return 0;
}
