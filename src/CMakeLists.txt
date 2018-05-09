set(CMAKE_INSTALL_RPATH "/usr/local/lib")
add_executable(vinbero vinbero.c vinbero_Core.c vinbero_Help.c vinbero_Options.c)
set_target_properties(vinbero PROPERTIES PUBLIC_HEADER "vinbero_Module.h;vinbero_ClData.h;vinbero_IModule.h;vinbero_ICLocal.h;vinbero_IClService.h;vinbero_ITlService.h;vinbero_Args.h;vinbero_ITLocal.h;vinbero_IBasic.h;vinbero_Config.h")
target_link_libraries(vinbero vinbero_log fastdl pthread jansson gaio)
install(TARGETS vinbero 
        RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include/vinbero)
