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

void __wrap_vinbero_common_Log_init(int flag) {}
void __wrap_vinbero_common_Log_raw(int level, const char* source, int line, const char* format, ...) {}
void __wrap_vinbero_Help_printAndExit() {}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vinbero_Options_process)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
