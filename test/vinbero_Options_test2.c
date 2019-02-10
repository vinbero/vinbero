#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <vinbero_com/vinbero_com_Error.h>
#include <vinbero_com/vinbero_com_Status.h>
#include <vinbero_com/vinbero_com_Config.h>
#include "../src/vinbero_Options.h"

void __wrap_vinbero_com_Log_init(int flag) {}
void __wrap_vinbero_com_Log_raw(int level, const char* source, int line, const char* format, ...) {}
void __wrap_vinbero_Help_print() {}
void __wrap_vinbero_Help_printAndExit() {}

void test_vinbero_Options_process(void** state) {
    char* args[] = {"vinbero", "-f", "63"};
    struct vinbero_com_Config config;
    vinbero_com_Config_init(&config);
    assert_int_equal(vinbero_Options_process(sizeof(args) / sizeof(char*), args, &config), VINBERO_COM_ERROR_INVALID_CONFIG);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vinbero_Options_process)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
