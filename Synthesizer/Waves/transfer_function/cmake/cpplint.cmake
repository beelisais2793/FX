# Additional target to perform cpplint run, requires cppcheck

if(UNIX AND MAKE_CPPLINT)
    # Get all project files
    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)

    # Remove files from 3rd party libraries
    foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
        # Exclude tests
        set(EXCUDED_DIR "test")
        string(FIND ${SOURCE_FILE} ${EXCUDED_DIR} PROJECT_TRDPARTY_DIR_FOUND)
        if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
            list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
        endif ()

        # Exclude eigen
        set(EXCUDED_DIR "eigen")
        string(FIND ${SOURCE_FILE} ${EXCUDED_DIR} PROJECT_TRDPARTY_DIR_FOUND)
        if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
            list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
        endif ()

        # Exclude other
        set(EXCUDED_DIR "CMakeCXXCompilerId.cpp")
        string(FIND ${SOURCE_FILE} ${EXCUDED_DIR} PROJECT_TRDPARTY_DIR_FOUND)
        if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
            list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
        endif ()
    endforeach ()


    # Run cpplint
    find_package(PythonInterp)
    if(NOT PYTHONINTERP_FOUND)
        message(WARNING "No python interpreter - cpplint failed")
        return()
    endif()

    find_program(CMAKE_CXX_CPPCLINT NAMES cpplint)

    if(CMAKE_CXX_CPPCLINT)
        add_custom_target(cpplint
            COMMAND ${CMAKE_CXX_CPPCLINT}
                --verbose=5
                --output=emacs
                ${ALL_SOURCE_FILES}
            DEPENDS ${ALL_SOURCE_FILES}
            COMMENT "Linting cpplint"
            VERBATIM
        )
    endif() #   CMAKE_CXX_CPPCLINT
endif() #   UNIX