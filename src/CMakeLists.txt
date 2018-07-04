set(CMAKE_INSTALL_RPATH "/usr/local/lib")
# Build and install libvinbero_core
add_library(vinbero_core SHARED vinbero_core.c)
list(APPEND vinbero_core_public_headers "vinbero_core.h")
set_target_properties(vinbero_core PROPERTIES VERSION 0.1.1
                                              SOVERSION 0.1
                                              PUBLIC_HEADER "${vinbero_core_public_headers}")
find_library(fastdl libfastdl.so REQUIRED)
find_library(gaio libgaio.so.0.3 REQUIRED)
target_link_libraries(vinbero_core ${fastdl} ${gaio} jansson pthread)
install(TARGETS vinbero_core 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/vinbero)

# Build and install vinbero)
add_executable(vinbero vinbero.c vinbero_Help.c vinbero_Options.c)
list(APPEND vinbero_public_headers "vinbero_interface_MODULE.h")
list(APPEND vinbero_public_headers "vinbero_interface_BASIC.h")
list(APPEND vinbero_public_headers "vinbero_interface_TLOCAL.h")
list(APPEND vinbero_public_headers "vinbero_interface_TLSERVICE.h")
list(APPEND vinbero_public_headers "vinbero_interface_CLOCAL.h")
list(APPEND vinbero_public_headers "vinbero_interface_CLSERVICE.h")
set_target_properties(vinbero PROPERTIES PUBLIC_HEADER "${vinbero_public_headers}")
find_library(vinbero_common libvinbero_common.so REQUIRED)
target_link_libraries(vinbero vinbero_core ${vinbero_common})
install(TARGETS vinbero 
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/vinbero)
