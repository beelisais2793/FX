option (PUGI_SHARED_LIB "Build shared instead of static library" ON)
option (PUGI_TESTS "Build tests" OFF)
option (PUGIXML_WCHAR_MODE "Enable wchar_t mode" OFF)
option (PUGIXML_COMPACT "Enable compact mode" OFF)
option (PUGIXML_NO_XPATH "Disable XPath" OFF)
option (PUGIXML_NO_EXCEPTIONS "Disable C++ exceptions" OFF)
set(PUGI_DEFINES "" CACHE STRING "Build defines")

# Pre-defines standard install locations on *nix systems.
include(GNUInstallDirs)
mark_as_advanced(CLEAR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

set(HEADERS "include/pugixml.hpp" "include/pugiconfig.hpp.in")
set(SOURCES ${HEADERS} "src/pugixml.cpp")

set (LIBRARY_NAME PUGIXML)
if (PUGI_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()
configure_file (${TEMPLATE_EXPORT_H} "include/pugiexport.hpp")
configure_file ("include/pugiconfig.hpp.in" "include/pugiconfig.hpp")
add_definitions (-DPUGIXML_BUILDING)
set(SOURCES ${SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/include/pugiexport.hpp")
set(SOURCES ${SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/include/pugiconfig.hpp")

unset (PUGIXML_INCLUDE_DIRS CACHE)
set (PUGIXML_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    CACHE STRING "pugiXML include directories")
include_directories (${PUGIXML_INCLUDE_DIRS})

unset (PUGIXML_LIBRARIES CACHE)
set (PUGIXML_LIBRARIES pugixml CACHE STRING "PugiXML library")

add_library(pugixml ${BUILD_TYPE} ${SOURCES})

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
#create_vcproj_userfile (pugixml)

# Enable C++11 long long for compilers that are capable of it
if(NOT ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS 3.1)
	target_compile_features(pugixml PUBLIC cxx_long_long_type)
endif()

set_target_properties(pugixml PROPERTIES VERSION 1.7 SOVERSION 1)

if (INSTALL_LIBS)
    install(TARGETS pugixml EXPORT pugixml-config
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin"
    )

    install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install (FILES "${CMAKE_CURRENT_BINARY_DIR}/include/pugiconfig.hpp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install (FILES "${CMAKE_CURRENT_BINARY_DIR}/include/pugiexport.hpp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT pugixml-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pugixml)
endif ()

if(PUGI_TESTS)
	file(GLOB TEST_SOURCES ../tests/*.cpp)
	file(GLOB FUZZ_SOURCES ../tests/fuzz_*.cpp)
	list(REMOVE_ITEM TEST_SOURCES ${FUZZ_SOURCES})

	add_executable(check ${TEST_SOURCES})
	target_link_libraries(check pugixml)
	add_custom_command(TARGET check POST_BUILD COMMAND check WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
endif()

