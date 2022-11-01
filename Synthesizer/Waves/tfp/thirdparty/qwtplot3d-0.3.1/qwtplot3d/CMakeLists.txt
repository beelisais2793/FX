###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-Wno-error -fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-Wno-error -fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# config
###############################################################################

# Required by export.hpp.in
set (LIBRARY_NAME "QWT3D")

if (QWTPLOT3D_SHARED_LIB)
    set (BUILD_TYPE "SHARED")
    add_definitions (-DQWT3D_BUILDING)
else ()
    set (BUILD_TYPE "STATIC")
endif ()

configure_file (${TEMPLATE_EXPORT_H} "include/qwt3d_export.h")
configure_file ("include/qwt3d_global.h.in" "include/qwt3d_global.h")

###############################################################################
# Library, source and header files
###############################################################################

set (QWTPLOT3D_SOURCES
    "src/qwt3d_appearance.cpp"
    "src/qwt3d_autoscaler.cpp"
    "src/qwt3d_axis.cpp"
    "src/qwt3d_color_std.cpp"
    "src/qwt3d_colorlegend.cpp"
    "src/qwt3d_coordsys.cpp"
    "src/qwt3d_data.cpp"
    "src/qwt3d_drawable.cpp"
    "src/qwt3d_enrichment_std.cpp"
    "src/qwt3d_extglwidget.cpp"
    "src/qwt3d_function.cpp"
    "src/qwt3d_graphplot.cpp"
    "src/qwt3d_gridmapping.cpp"
    "src/qwt3d_gridplot.cpp"
    "src/qwt3d_io.cpp"
    "src/qwt3d_io_reader.cpp"
    "src/qwt3d_label.cpp"
    "src/qwt3d_lighting.cpp"
    "src/qwt3d_meshplot.cpp"
    "src/qwt3d_parametricsurface.cpp"
    "src/qwt3d_plot3d.cpp"
    "src/qwt3d_scale.cpp"
    "src/qwt3d_surfaceplot.cpp"
    "src/qwt3d_types.cpp"
    "src/qwt3d_volumeplot.cpp")

set (QWTPLOT3D_MOC_HEADERS
    "include/qwt3d_plot3d.h"
    "include/qwt3d_extglwidget.h"
    "include/qwt3d_gridplot.h"
    "include/qwt3d_surfaceplot.h")

find_package (Qt5 COMPONENTS Core Gui OpenGL REQUIRED)

qt5_wrap_cpp (QWTPLOT3D_MOC_SOURCES ${QWTPLOT3D_MOC_HEADERS})

set (LIBRARY_NAME QWTPLOT3D)
if (QWTPLOT3D_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()

add_library (qwtplot3d ${BUILD_TYPE}
    ${QWTPLOT3D_SOURCES}
    ${QWTPLOT3D_MOC_SOURCES}
)

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (qwtplot3d)

target_link_libraries (qwtplot3d Qt5::Core Qt5::Gui Qt5::OpenGL)

find_package (OpenGL REQUIRED)
list (APPEND QWTPLOT3D_INCLUDE_DIRS "${OPENGL_INCLUDE_DIR}")
target_link_libraries (qwtplot3d ${OPENGL_LIBRARIES})

list (APPEND QWTPLOT3D_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND QWTPLOT3D_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include") # qwt3d_export.h, qwt3d_global.h

include_directories (${QWTPLOT3D_INCLUDE_DIRS})
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include") # qwt3d_export.h, qwt3d_global.h

unset (QWTPLOT3D_INCLUDE_DIRS CACHE)
set (QWTPLOT3D_INCLUDE_DIRS ${QWTPLOT3D_INCLUDE_DIRS} CACHE STRING "Qwt include directories")
unset (QWTPLOT3D_LIBRARIES CACHE)
set (QWTPLOT3D_LIBRARIES
    ${OPENGL_glu_LIBRARY}
    ${ZLIB_LIBRARIES}
    qwtplot3d
    CACHE STRING "qwtplot3d libraries")

###############################################################################
# install targets
###############################################################################

install (
    TARGETS qwtplot3d
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)

