add_executable(vinbero_Options_test vinbero_Options_test.c ../src/vinbero_Options.c)
target_link_libraries(vinbero_Options_test vinbero_core vinbero_common cmocka "-Wl,--wrap,vinbero_common_Log_raw -Wl,--wrap,vinbero_common_Log_init -Wl,--wrap,vinbero_Help_printAndExit")
add_test(NAME vinbero_Options_test COMMAND vinbero_Options_test)
