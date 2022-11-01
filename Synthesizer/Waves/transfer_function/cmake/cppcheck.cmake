# Additional target to perform cppcheck run, requires cppcheck

if(UNIX AND MAKE_CPPCHECK)
    # Get all project files
    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)

    # Remove files from 3rd party libraries
    foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
        # Exclude tests
        set(EXCUDED_DIR "tests")
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


    # Run CppCheck
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)

    if(CMAKE_CXX_CPPCHECK)
        add_custom_target(cppcheck
            COMMAND ${CMAKE_CXX_CPPCHECK}
                --enable=all#warning,performance,portability,information,missingInclude
                --std=c++20
                --inconclusive
                #--library=qt.cfg
                #--template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
                --verbose
                --inline-suppr
                --force
                #--output-file=${CMAKE_SOURCE_DIR}/cppcheck_output.txt
                --suppress=missingInclude
                --xml
                --xml-version=2
                2> ${CMAKE_CURRENT_BINARY_DIR}/cppcheck_output.xml
                ${ALL_SOURCE_FILES}
        )

        add_custom_target(cppcheck_html
            COMMAND cppcheck-htmlreport
                --file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck_output.xml
                --report-dir=${CMAKE_CURRENT_BINARY_DIR}/cppcheck_output_html
                --source-dir=.
        )
    endif() #   CMAKE_CXX_CPPCHECK
endif() # UNIX