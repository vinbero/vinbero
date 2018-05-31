set(CMAKE_INSTALL_RPATH "/usr/local/lib")

add_library(vinbero_core SHARED vinbero_core.c)
install(TARGETS vinbero_core LIBRARY DESTINATION lib)

add_executable(vinbero vinbero.c vinbero_Help.c vinbero_Options.c)
set_target_properties(vinbero PROPERTIES PUBLIC_HEADER "vinbero_core.h;vinbero_Interface_MODULE.h;vinbero_Interface_CLOCAL.h;vinbero_Interface_CLSERVICE.h;vinbero_Interface_TLSERVICE.h;vinbero_Interface_TLOCAL.h;vinbero_Interface_BASIC.h")
target_link_libraries(vinbero vinbero_core vinbero_common jansson fastdl pthread gaio)
install(TARGETS vinbero 
        RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include/vinbero)
