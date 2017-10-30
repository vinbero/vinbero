#include <jansson.h>
#include <libgenc/genc_List.h>
#include "tucube_Core.h"
#include "tucube_Options.h"

int main(int argc, char* argv[])
{
    struct tucube_Core core;
    struct tucube_Config config;
    GENC_LIST_INIT(&moduleConfigList);
    tucube_Options_process(argc, argv, &config);
    tucube_Core_start(&core, &config);
    json_decref(config.json);
    return 0;
}
