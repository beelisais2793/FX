###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# config
###############################################################################

set (QWT3D_MAJOR_VERSION 0)
set (QWT3D_MINOR_VERSION 3)
set (QWT3D_PATCH_VERSION 1)
set (QWT3D_VERSION "${QWT3D_MAJOR_VERSION}.${QWT3D_MINOR_VERSION}.${QWT3D_PATCH_VERSION}")

option (QWTPLOT3D_SHARED_LIB "Enable this to build Qwt as a shared library. Disable to build as a static library" ON)

message (STATUS "------------------------------------------------------------")
message (STATUS "qwtplot3d settings")
message (STATUS " + Build as a shared library: ${QWTPLOT3D_SHARED_LIB}")
message (STATUS "------------------------------------------------------------")

###############################################################################
# sub directories
###############################################################################

add_subdirectory ("qwtplot3d")
