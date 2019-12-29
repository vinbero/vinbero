#include "vinbero_Help.h"
#include <stdio.h>
#include <stdlib.h>

void
vinbero_Help_print() {
    puts("Usage: vinbero [OPTION]...");
    puts("A Modular Server.");
    puts("Options:");
    puts("  -i --inline-config       Inline JSON-based config.");
    puts("  -c --config-file         JSON-based config file.");
    puts("  -f --logging-flag        Set logging level flag.");
    puts("  -o --logging-option      Set logging option.");
    puts("  -v --version             Print version info.");
    puts("  -h --help                Print this help message.");
}
