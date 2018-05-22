#include <vinbero_common/vinbero_common_Config.h>
#include "../src/vinbero_Options.h"
/*
int vinbero_Options_process(int argc, char* argv[], struct vinbero_common_Config* config) {
    int ret;
    struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"inline-config", required_argument, NULL, 'i'},
        {"config-file", required_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
    };
    json_error_t configError;
    char optionChar;
    bool optionsExist = false;
    while((optionChar = getopt_long(argc, argv, "hi:f:", options, NULL)) != (char)-1) {
        optionsExist = true;
        switch(optionChar) {
        case 'i':
            if((ret = vinbero_common_Config_fromString(config, optarg)) < 0) {
                return ret;
            }
            break;
        case 'f':
            if((ret = vinbero_common_Config_fromFile(config, optarg)) < 0) {
                return ret;
            }
            break;
        case 'h':
        default:
            vinbero_Help_printAndExit();
            break;
        }
    }

    if(!optionsExist)
        vinbero_Help_printAndExit();

    if(config->json == NULL)
        vinbero_Help_printAndExit();

    return 0;

}*/

int __wrap_vinbero_common_Config_fromString(struct vinbero_common_Config* config, const char* input) {
    return 0;
}
int __wrap_vinbero_common_Config_fromFile(struct vinbero_common_Config* config, const char* path) {
    return 0;
}
void __wrap_vinbero_Help_printAndExit() {
}

int main() {
    struct vinbero_common_Config config;
    char* args[] =  {"-f", "config.json"};
    vinbero_Options_process(2, args, &config);
}
