#include <stdio.h>
#include "tucube_Help.h"

void tucube_Help_print() {
    puts("Usage: tucube [OPTION]...");
    puts("A Modular TCP/UDP Server.");
    puts("Options:");
    puts("  -h --help                Print this help message.");
    puts("  -i --inline-config       Inline JSON-based config.");
    puts("  -f --config-file         JSON-based config file.");
}
