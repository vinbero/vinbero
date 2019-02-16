set(CMAKE_INSTALL_RPATH "\$ORIGIN:/usr/local/lib")

# Build and install libvinbero_core
add_library(vinbero_core SHARED vinbero_core.c)
list(APPEND public_headers "vinbero_core.h")
set_target_properties(
    vinbero_core
    PROPERTIES
    VERSION ${version}
    SOVERSION ${soversion}
    PUBLIC_HEADER "${public_headers}"
)
target_compile_options(vinbero_core PRIVATE -Wall)
find_library(fastdl libfastdl.so REQUIRED)
find_library(gaio libgaio.so REQUIRED)
target_link_libraries(vinbero_core ${fastdl} ${gaio} pthread dl)
install(TARGETS vinbero_core 
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/vinbero)

# Build and install vinbero)
add_executable(vinbero vinbero.c vinbero_Help.c vinbero_Options.c)
find_library(vinbero_com libvinbero_com.so REQUIRED)
target_link_libraries(vinbero vinbero_core ${vinbero_com})
install(TARGETS vinbero 
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/vinbero)
