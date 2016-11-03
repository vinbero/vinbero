#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgonc/gonc_list.h>
#include "config.h"
#include "tucube_Master.h"
#include "tucube_Options.h"

int main(int argc, char* argv[])
{
/*    struct tucube_Master* master = malloc(1 * sizeof(struct tucube_Master));
    master->moduleArgsList = malloc(1 * sizeof(struct tucube_Module_ArgsList));
    GONC_LIST_INIT(master->moduleArgsList);

    master->setUid = geteuid();
    master->setGid = getegid();
    master->address = "0.0.0.0";
    master->port = 8080;
    master->backlog = 1024;
    master->reusePort = 0;
    master->workerCount = 4;

    tucube_Options_process(argc, argv, coreConfig, master->moduleConfigList);

    tucube_Master_initCore(master);

    if(setgid(master->setGid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if(setuid(master->setUid) == -1)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    tucube_Master_initModules(master);

    tucube_Master_start(master);

    free(master);
    */

    struct tucube_Core core;
    struct tucube_Core_Config coreConfig;
    struct tucube_Module_ConfigList moduleConfigList;
    GONC_LIST_INIT(&moduleConfigList);
    tucube_Options_process(argc, argv, &coreConfig, &moduleConfigList);
    tucube_Core_init(&core, &coreConfig, &moduleConfigList);
    tucube_Core_start(&core);
    return 0;
}
