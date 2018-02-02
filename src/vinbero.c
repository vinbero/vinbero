#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include <string.h>
#include "vinbero_Core.h"
#include "vinbero_Options.h"

int main(int argc, char* argv[]) {
    struct vinbero_Module module;
    memset(&module, 0, sizeof(struct vinbero_Module));
    GENC_TREE_NODE_INIT(&module);
    GENC_TREE_NODE_INIT(&module.interface);
    module.id = "core";
    module.localModule.pointer = malloc(1 * sizeof(struct vinbero_Core));
    module.interface = malloc(1 * sizeof(struct vinbero_Core_Interface));
    struct vinbero_Config config;
    config.json = NULL;
    vinbero_Options_process(argc, argv, &config);
    vinbero_Core_start(&module, &config);
    json_decref(config.json);
    free(module.localModule.pointer);
    free(module.interface);
    return 0;
}
