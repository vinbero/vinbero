#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <vinbero_com/vinbero_com_Status.h>
#include <vinbero_com/vinbero_com_Config.h>
#include "../src/vinbero_core.h"

void __wrap_vinbero_com_Log_init(int flag) {}
void __wrap_vinbero_com_Log_raw(int level, const char* source, int line, const char* format, ...) {}
void __wrap_vinbero_Help_print() {}
void __wrap_vinbero_Help_printAndExit() {}

void test_vinbero_core(void** state) {
    struct vinbero_com_Config config;
    vinbero_com_Config_init(&config);
    vinbero_com_Config_fromFile(&config, "test.json");
    assert_int_equal(vinbero_core_checkConfig(&config, "core"), VINBERO_COM_STATUS_SUCCESS);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vinbero_core)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
