#include <stdio.h>
#include "tucube_Help.h"

void tucube_Help_print() {
    printf("Usage: tucube [OPTION]...\n");
    printf("A Modular TCP/UDP Server.\n");
    printf("Options:\n");
    printf("  -h --help                Print this help message.\n");
    printf("  -u --set-uid             Set Real & Effective User ID(string/number). Default: current euid\n");
    printf("  -g --set-gid             Set Real & Effective Group ID(string/number). Default: current egid\n");
    printf("  -a --address             Set IP address. Default: 0.0.0.0\n");
    printf("  -p --port                Set port number. Default: 8080\n");
    printf("  -b --backlog             Set backlog size. Default: 1024\n");
    printf("  -r --reuse-port          Set reuse_port(0/1). Default: 0\n");
    printf("  -w --worker-count        Set worker count. Default: 4\n");
    printf("  -m --module-args         Set module arguments. You can use this option multiple times.\n                             Order of multiple --module-args is preserved. First argument must be the path of a module.\n                             Arguments are separated by comma(without spaces), name and value of argument are separated by colon(without spaces).\n                             First argument doesn't need name, it only needs value(the path of module).\n");
}
