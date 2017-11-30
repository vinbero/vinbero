#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include "tucube_Core.h"
#include "tucube_Options.h"

int main(int argc, char* argv[]) {
    struct tucube_Module module;
    GENC_TREE_NODE_INIT(&module);
    module.name = "core";
    module.localModule.pointer = malloc(1 * sizeof(struct tucube_Core));
    struct tucube_Config config;
    config.json = NULL;
    tucube_Options_process(argc, argv, &config);
    tucube_Core_start(&module, &config);
    json_decref(config.json);
    free(module.localModule.pointer);
    return 0;
}
