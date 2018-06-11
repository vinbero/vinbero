#include <stdio.h>
#include <stdlib.h>
#include "vinbero_Help.h"

void vinbero_Help_print() {
    puts("Usage: vinbero [OPTION]...");
    puts("A Modular TCP/UDP Server.");
    puts("Options:");
    puts("  -h --help                Print this help message.");
    puts("  -i --inline-config       Inline JSON-based config.");
    puts("  -f --config-file         JSON-based config file.");
}
