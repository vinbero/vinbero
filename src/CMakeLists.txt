set(CMAKE_INSTALL_RPATH "/usr/local/lib")

add_library(vinbero_core SHARED vinbero_core.c)
install(TARGETS vinbero_core LIBRARY DESTINATION lib)

add_executable(vinbero vinbero.c vinbero_Help.c vinbero_Options.c)
set_target_properties(vinbero PROPERTIES PUBLIC_HEADER "vinbero_core.h;vinbero_ClData.h;vinbero_IModule.h;vinbero_ICLocal.h;vinbero_IClService.h;vinbero_ITlService.h;vinbero_ITLocal.h;vinbero_IBasic.h")
target_link_libraries(vinbero vinbero_core vinbero_common jansson fastdl pthread gaio)
install(TARGETS vinbero 
        RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include/vinbero)
