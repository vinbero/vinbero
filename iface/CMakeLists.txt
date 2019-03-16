FILE(GLOB dirs RELATIVE "${CMAKE_SOURCE_DIR}/iface" "*")

FOREACH(dir ${dirs})
  IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/iface/${dir}" AND (NOT ${dir} STREQUAL "${CMAKE_SOURCE_DIR}/iface"))
      add_subdirectory(${dir})
  ENDIF()
ENDFOREACH()
