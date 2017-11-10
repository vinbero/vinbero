#include <stdio.h>
#include <stdlib.h>
#include "tucube_Help.h"

void tucube_Help_print() {
    puts("Usage: tucube [OPTION]...");
    puts("A Modular TCP/UDP Server.");
    puts("Options:");
    puts("  -h --help                Print this help message.");
    puts("  -i --inline-config       Inline JSON-based config.");
    puts("  -f --config-file         JSON-based config file.");
}

void tucube_Help_printAndExit() {
    tucube_Help_print();
    exit(EXIT_SUCCESS);
}
