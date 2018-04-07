#include <jansson.h>
#include <libgenc/genc_Tree.h>
#include <string.h>
#include "vinbero_Core.h"
#include "vinbero_Options.h"

int main(int argc, char* argv[]) {
    struct vinbero_Config config;
    config.json = NULL;
    vinbero_Options_process(argc, argv, &config);
    vinbero_Core_start(&config);
    json_decref(config.json);
    return 0;
}
