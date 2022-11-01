macro(BuildExample example_src dir name_prefix linklibs)
    get_filename_component(EXAMPLE_NAME ${example_src} NAME_WE) # Get name w/o extension
    get_filename_component(EXAMPLE_DIRECTORY ${example_src} DIRECTORY) # Get name w/o extension

    if ("${dir}" STREQUAL ".")
        set(EXAMPLE_TARGET ${name_prefix}_examples_${EXAMPLE_NAME})
    else()
        set(EXAMPLE_TARGET ${name_prefix}_examples_${dir}_${EXAMPLE_NAME})
    endif()

    add_executable(${EXAMPLE_TARGET} ${example_src})

    set_target_properties(${EXAMPLE_TARGET}
        PROPERTIES
        DEBUG_POSTFIX d
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON

#        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_DIRECTORY}/bin
#        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXAMPLE_DIRECTORY}/bin
#        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXAMPLE_DIRECTORY}/bin
        )

    #       message("Adding target for example: ${example_src}")
    include_directories(${ALLOCORE_INCLUDE_DIR} ${GAMMA_INCLUDE_DIRS}})
    #    message("Gamma : ${GAMMA_INCLUDE_DIRs}")
    add_dependencies(${EXAMPLE_TARGET} al Gamma)
    target_link_libraries(${EXAMPLE_TARGET} al ${linklibs} ${OPENGL_gl_LIBRARY} ${ADDITIONAL_LIBRARIES} "${name_prefix}")

    get_target_property(DLLS_TO_COPY al AL_DLL_LIBRARIES)
    Copy_dlls("${EXAMPLE_DIRECTORY}/bin" "${EXAMPLE_TARGET}" "${EXTENSIONS_DLLS}")

endmacro()

#${CMAKE_CURRENT_SOURCE_DIR}/examples
macro(BuildExamples directory name_prefix linklibs)
    subdirlist(EXAMPLE_DIRS "${directory}")
    #include root projects directory too
    list(APPEND EXAMPLE_DIRS ".")
    foreach(dir ${EXAMPLE_DIRS})
        file(GLOB EXAMPLE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} examples/${dir}/*.cpp)

        foreach(example_src ${EXAMPLE_FILES})
            list (FIND EXAMPLES_TO_IGNORE "${example_src}" _index)
            if (${_index} EQUAL -1)
                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples/bin")
                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/examples/bin")
                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/examples/bin")
                BuildExample("${example_src}" "${dir}" "${name_prefix}" "${linklibs}" )

            else()
                message("Ignoring example ${EXAMPLE_NAME}")
            endif (${_index} EQUAL -1)
        endforeach(example_src)

    endforeach(dir)

    foreach(example_src ${EXTENSIONS_EXAMPLES})
        #    message("Building extension example: ${example_src}")
        get_filename_component(EXAMPLE_DIR "${example_src}" DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXAMPLE_DIR}/bin")
        BuildExample("${example_src}" "extensions" "${name_prefix}" "${linklibs}" )
    endforeach()

endmacro()
