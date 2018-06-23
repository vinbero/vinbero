#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <vinbero_common/vinbero_common_Status.h>
#include <vinbero_common/vinbero_common_Config.h>
#include "../src/vinbero_core.h"

void __wrap_vinbero_common_Log_init(int flag) {}
void __wrap_vinbero_common_Log_raw(int level, const char* source, int line, const char* format, ...) {}
void __wrap_vinbero_Help_print() {}
void __wrap_vinbero_Help_printAndExit() {}

void test_vinbero_core(void** state) {
    struct vinbero_common_Config config;
    vinbero_common_Config_init(&config);
    vinbero_common_Config_fromFile(&config, "test.json");
    assert_int_equal(vinbero_core_checkConfig(&config, "core"), VINBERO_COMMON_STATUS_SUCCESS);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vinbero_core)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
