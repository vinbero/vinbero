add_executable(vinbero_Options_test vinbero_Options_test.c ../src/vinbero_Options.c)
target_link_libraries(vinbero_Options_test cmocka)
add_test(NAME vinbero_Options_test COMMAND vinbero_Options_test)
