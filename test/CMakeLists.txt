add_executable(vinbero_Options_test vinbero_Options_test.c ../src/vinbero_Options.c)
target_link_libraries(vinbero_Options_test vinbero_core vinbero_common cmocka "-Wl,--wrap,vinbero_common_Log_raw -Wl,--wrap,vinbero_common_Log_init -Wl,--wrap,vinbero_Help_printAndExit -Wl,-wrap,vinbero_Help_print")

add_executable(vinbero_Options_test2 vinbero_Options_test2.c ../src/vinbero_Options.c)
target_link_libraries(vinbero_Options_test2 vinbero_core vinbero_common cmocka "-Wl,--wrap,vinbero_common_Log_raw -Wl,--wrap,vinbero_common_Log_init -Wl,--wrap,vinbero_Help_printAndExit -Wl,-wrap,vinbero_Help_print")

add_test(NAME vinbero_Options_test COMMAND vinbero_Options_test)
add_test(NAME vinbero_Options_test2 COMMAND vinbero_Options_test2)
