#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <vinbero_common/vinbero_common_Error.h>
#include <vinbero_common/vinbero_common_Status.h>
#include <vinbero_common/vinbero_common_Config.h>
#include "../src/vinbero_Options.h"

void __wrap_vinbero_common_Log_init(int flag) {}
void __wrap_vinbero_common_Log_raw(int level, const char* source, int line, const char* format, ...) {}
void __wrap_vinbero_Help_print() {}
void __wrap_vinbero_Help_printAndExit() {}

void test_vinbero_Options_process(void** state) {
    char* args[] = {"vinbero", "-f", "63", "-c", "test.json"};
    struct vinbero_common_Config config;
    vinbero_common_Config_init(&config);
    assert_int_equal(vinbero_Options_process(sizeof(args) / sizeof(char*), args, &config), VINBERO_COMMON_STATUS_SUCCESS);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vinbero_Options_process)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
