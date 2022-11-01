function (add_plugin TARGET)
    set (multiValueArgs SOURCES INCLUDE_DIRS HEADERS TEST_SOURCES FORMS RESOURCES TRANSLATIONS)
    cmake_parse_arguments (add_plugin "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    option (TFP_ENABLE_${TARGET} "Whether or not to build the ${TARGET} plugin" ON)
    if (NOT TFP_ENABLE_${TARGET})
        return ()
    endif ()

    if (NOT TARGET)
        message (FATAL_ERROR "Require a target name to add_plugin() call: add_plugin(<target name> [...])")
    endif ()

    find_package (Qt5 COMPONENTS Core Widgets Gui REQUIRED)

    if (add_plugin_HEADERS)
        qt5_wrap_cpp (SOURCES_MOC ${add_plugin_HEADERS})
    endif ()
    if (add_plugin_FORMS)
        qt5_wrap_ui  (FORMS_MOC ${add_plugin_FORMS})
    endif ()
    if (add_plugin_RESOURCES)
        qt5_add_resources (RESOURCES_GEN ${add_plugin_RESOURCES})
    endif ()
    if (add_plugin_TRANSLATIONS)
        qt5_add_translation (QM_FILES ${add_plugin_TRANSLATIONS})
    endif ()

    add_definitions (-DPLUGIN_BUILDING)
    include_directories (${add_plugin_INCLUDE_DIRS})

    set (test_SOURCES "")
    if (TFP_BUILD_TESTS AND add_plugin_TEST_SOURCES)
        message (STATUS "Plugin ${TARGET} has tests")
        configure_file ("${CMAKE_SOURCE_DIR}/CMake/Templates/PluginTestMain.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/PluginTestMain.cpp")
        configure_file ("${CMAKE_SOURCE_DIR}/CMake/Templates/PluginTestAPI.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/PluginTestAPI.cpp")
        set (test_SOURCES ${add_plugin_TEST_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/PluginTestAPI.cpp")
        add_executable (test_${TARGET} "${CMAKE_CURRENT_BINARY_DIR}/PluginTestMain.cpp")
    endif ()

    add_library (${TARGET} SHARED
        ${add_plugin_HEADERS}
        ${add_plugin_SOURCES}
        ${SOURCES_MOC}
        ${FORMS_MOC}
        ${RESOURCES_GEN}
        ${QM_FILES}
        ${test_SOURCES}
    )

    set_target_properties (${TARGET}
        PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
        PREFIX "")

    ###############################################################################
    # dependencies
    ###############################################################################

    add_dependencies (tfp_application ${TARGET})

    target_link_libraries (${TARGET} Qt5::Core)
    target_link_libraries (${TARGET} Qt5::Widgets)
    target_link_libraries (${TARGET} Qt5::Gui)
    #target_link_libraries (${TARGET} Qt5::Concurrent)
    #target_link_libraries (${TARGET} Qt5::PrintSupport)
    #target_link_libraries (${TARGET} Qt5::OpenGL)
    #target_link_libraries (${TARGET} Qt5::Svg)

    target_link_libraries (${TARGET} qwtinternal)
    target_link_libraries (${TARGET} qwtplot3d)
    target_link_libraries (${TARGET} kiss_fft)
    target_link_libraries (${TARGET} pugixml)
    target_link_libraries (${TARGET} tfp)

    if (TFP_BUILD_TESTS AND add_plugin_TEST_SOURCES)
        target_link_libraries (test_${TARGET} gmock)
        target_link_libraries (test_${TARGET} Qt5::Core)
        target_link_libraries (test_${TARGET} Qt5::Widgets)
        target_link_libraries (test_${TARGET} Qt5::Gui)
        target_link_libraries (test_${TARGET} tfp)
    endif ()
endfunction ()
