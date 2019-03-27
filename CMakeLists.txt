cmake_minimum_required(VERSION 3.5)
include(cmake/gonapps-cmake-helper.cmake)
project(vinbero C)
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

gonapps_setVersion("0.3.1" "0.3")
gonapps_setBuildType()

add_subdirectory(docker)
add_subdirectory(iface)
add_subdirectory(src)
enable_testing()
add_subdirectory(test)
