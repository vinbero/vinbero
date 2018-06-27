set(CMAKE_INSTALL_RPATH "/usr/local/lib")

add_library(vinbero_core SHARED vinbero_core.c)
list(APPEND vinbero_core_public_headers "vinbero_core.h")
set_target_properties(vinbero_core PROPERTIES
                                   VERSION 0.1.1
                                   SOVERSION 0.1
                                   PUBLIC_HEADER "${vinbero_core_public_headers}")
target_link_libraries(vinbero_core jansson fastdl pthread gaio)
install(TARGETS vinbero_core 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/vinbero)

add_executable(vinbero vinbero.c vinbero_Help.c vinbero_Options.c)
list(APPEND vinbero_public_headers "vinbero_interface_MODULE.h")
list(APPEND vinbero_public_headers "vinbero_interface_BASIC.h")
list(APPEND vinbero_public_headers "vinbero_interface_TLOCAL.h")
list(APPEND vinbero_public_headers "vinbero_interface_TLSERVICE.h")
list(APPEND vinbero_public_headers "vinbero_interface_CLOCAL.h")
list(APPEND vinbero_public_headers "vinbero_interface_CLSERVICE.h")
set_target_properties(vinbero PROPERTIES
                              PUBLIC_HEADER "${vinbero_public_headers}")
target_link_libraries(vinbero vinbero_core vinbero_common)
install(TARGETS vinbero 
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/vinbero)
