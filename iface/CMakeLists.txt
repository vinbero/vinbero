file(GLOB dirs RELATIVE "${PROJECT_SOURCE_DIR}/iface" "*")

foreach(dir ${dirs})
  if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/iface/${dir}" AND (NOT ${dir} STREQUAL "${PROJECT_SOURCE_DIR}/iface"))
      add_subdirectory(${dir})
  endif()
endforeach()
