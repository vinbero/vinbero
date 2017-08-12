set(CMAKE_INSTALL_RPATH "/usr/local/lib")
add_executable(tucube tucube.c tucube_Core.c tucube_Help.c tucube_Options.c)
set_target_properties(tucube PROPERTIES PUBLIC_HEADER "tucube_Module.h;tucube_IBase.h;tucube_ICLocal.h;tucube_IClService.h;tucube_ITlService.h;tucube_ClData.h")
target_link_libraries(tucube dl pthread jansson gaio)
install(TARGETS tucube 
        RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include/tucube)
