#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>

#include <vinbero_common/vinbero_common_Config.h>
#include "../src/vinbero_Options.h"


void test_vinbero_Options_process(void** state) {
    char* args[] = {"-l", "63"};
    vinbero_Options_process(2, args, NULL);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vinbero_Options_process)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
