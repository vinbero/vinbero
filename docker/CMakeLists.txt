FILE(GLOB dirs RELATIVE "${PROJECT_SOURCE_DIR}/docker" "*")

foreach(dir ${dirs})
    if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/docker/${dir}" AND (NOT ${dir} STREQUAL "${PROJECT_SOURCE_DIR}/docker"))
        get_filename_component(name ${dir} NAME_WE)
        message(${name})
        message(${dir}/Dockerfile)
        add_custom_target(docker_${name} ALL COMMAND gmcr < "${PROJECT_SOURCE_DIR}/docker/Dockerfile.gmcr" -a "${PROJECT_SOURCE_DIR}/docker/${name}.json" > "${PROJECT_SOURCE_DIR}/docker/${name}/Dockerfile")
    endif()
endforeach()

#add_custom_target(
    #    docker ALL
    #    DEPENDS "${docker_files}"
    #)

    #foreach(docker_dir ${docker_dirs})
    #    message("hey there ${docker_dir}/Dockerfile")
    #    add_custom_command(
    #        OUTPUT "${docker_dir}/Dockerfile"
    #        COMMAND gmcr < ${PROJECT_SOURCE_DIR}/docker/Dockerfile.gmcr -a ${docker_dir}.json > ${docker_dir}/Dockerfile
    #        COMMENT "generating ${docker_dir}/Dockerfile"
    #    )
    #endforeach()
