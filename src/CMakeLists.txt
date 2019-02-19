find_package(fastdl REQUIRED)
find_package(gaio REQUIRED)
find_package(vinbero_com REQUIRED)

add_library(vinbero_core SHARED)
target_sources(vinbero_core PRIVATE vinbero_core.c)
target_compile_options(vinbero_core PRIVATE -Wall)
list(APPEND public_headers "vinbero_core.h")

set_target_properties(
    vinbero_core
    PROPERTIES
    C_STANDARD 11
    VERSION ${version}
    SOVERSION ${soversion}
    PUBLIC_HEADER "${public_headers}"
)

target_include_directories(
    vinbero_core
    PUBLIC
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
    vinbero_core
    vinbero_com::vinbero_com
    fastdl::fastdl
    gaio::gaio
    pthread
    dl
)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
    @ONLY
)

install(
    TARGETS vinbero_core 
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/vinbero
)

add_executable(vinbero)
target_sources(vinbero PRIVATE vinbero.c vinbero_Help.c vinbero_Options.c)
set_target_properties(
    vinbero
    PROPERTIES
    C_STANDARD 11
    INSTALL_RPATH "\$ORIGIN:/usr/local/lib"
)

target_link_libraries(
    vinbero
    vinbero_core
)

install(
    TARGETS vinbero 
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/vinbero
)
