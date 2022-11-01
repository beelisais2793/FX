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
# Defines
###############################################################################

if (QWT_SHARED)
    add_definitions (-DQT_DLL -DQWT_DLL -DQWT_MAKEDLL)
endif ()

if (QWT_SVG)
    add_definitions (-DQWT_SVG)
else ()
    add_definitions (-DQWT_NO_SVG)
endif ()

if (NOT QWT_OPENGL)
    add_definitions (-DQWT_NO_OPENGL)
endif ()

###############################################################################
# Library, source and header files
###############################################################################

file (GLOB QWT_HEADERS "core/include/*.h")
file (GLOB QWT_SOURCES "core/src/*.cpp")
set (QWT_MOC_HEADERS
    "core/include/qwt_dyngrid_layout.h"
    "core/include/qwt_magnifier.h"
    "core/include/qwt_panner.h"
    "core/include/qwt_picker.h"
    "core/include/qwt_text_label.h")
set (QWT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/core/include")

if (QWT_PLOT)
    file (GLOB QWT_PLOT_HEADERS "plot/include/*.h")
    file (GLOB QWT_PLOT_SOURCES "plot/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_PLOT_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_PLOT_SOURCES})
    list (APPEND QWT_MOC_HEADERS
        "plot/include/qwt_abstract_legend.h"
        "plot/include/qwt_legend.h"
        "plot/include/qwt_legend_label.h"
        "plot/include/qwt_plot.h"
        "plot/include/qwt_plot_renderer.h"
        "plot/include/qwt_plot_canvas.h"
        "plot/include/qwt_plot_panner.h"
        "plot/include/qwt_plot_picker.h"
        "plot/include/qwt_plot_zoomer.h"
        "plot/include/qwt_plot_magnifier.h"
        "plot/include/qwt_sampling_thread.h"
        "plot/include/qwt_scale_widget.h")
    list (APPEND QWT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/plot/include")
endif ()
if (QWT_SVG)
    file (GLOB QWT_SVG_HEADERS "svg/include/*.h")
    file (GLOB QWT_SVG_SOURCES "svg/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_SVG_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_SVG_SOURCES})
    list (APPEND QWT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/svg/include")
endif ()
if (QWT_OPENGL)
    file (GLOB QWT_OPENGL_HEADERS "opengl/include/*.h")
    file (GLOB QWT_OPENGL_SOURCES "opengl/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_OPENGL_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_OPENGL_SOURCES})
    list (APPEND QWT_MOC_HEADERS "opengl/include/qwt_plot_glcanvas.h")
    list (APPEND QWT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/opengl/include")
endif ()
if (QWT_WIDGETS)
    file (GLOB QWT_WIDGETS_HEADERS "widgets/include/*.h")
    file (GLOB QWT_WIDGETS_SOURCES "widgets/src/*.cpp")
    list (APPEND QWT_HEADERS ${QWT_WIDGETS_HEADERS})
    list (APPEND QWT_SOURCES ${QWT_WIDGETS_SOURCES})
    list (APPEND QWT_MOC_HEADERS
        "widgets/include/qwt_abstract_slider.h"
        "widgets/include/qwt_abstract_scale.h"
        "widgets/include/qwt_analog_clock.h"
        "widgets/include/qwt_compass.h"
        "widgets/include/qwt_counter.h"
        "widgets/include/qwt_dial.h"
        "widgets/include/qwt_knob.h"
        "widgets/include/qwt_slider.h"
        "widgets/include/qwt_thermo.h"
        "widgets/include/qwt_wheel.h")
    list (APPEND QWT_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/widgets/include")
endif ()

find_package (Qt5 COMPONENTS Core Concurrent PrintSupport REQUIRED)
qt5_wrap_cpp (QWT_MOC_SOURCES ${QWT_MOC_HEADERS})

set (LIBRARY_NAME QWT)
if (QWT_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()
configure_file (${TEMPLATE_EXPORT_H} "include/qwt_export.h")
list (APPEND QWT_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include")

unset (QWT_INCLUDE_DIRS CACHE)
set (QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIRS} CACHE STRING "Qwt include directories")
include_directories (${QWT_INCLUDE_DIRS})

add_library (qwtinternal ${BUILD_TYPE}
    ${QWT_SOURCES}
    ${QWT_MOC_SOURCES}
)
unset (QWT_LIBRARIES CACHE)
set (QWT_LIBRARIES qwtinternal CACHE STRING "Qwt libraries")

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (qwtinternal)

add_definitions (-DQWT_BUILDING)

target_link_libraries (qwtinternal Qt5::Core Qt5::Concurrent Qt5::PrintSupport)

if (QWT_WIDGETS)
    find_package (Qt5 COMPONENTS Widgets REQUIRED)
    target_link_libraries (qwtinternal Qt5::Widgets)
endif ()

if (QWT_SVG)
    find_package (Qt5 COMPONENTS Svg REQUIRED)
    target_link_libraries (qwtinternal Qt5::Svg)
endif ()

if (QWT_OPENGL)
    find_package (Qt5 COMPONENTS OpenGL REQUIRED)
    target_link_libraries (qwtinternal Qt5::OpenGL)
endif ()

###############################################################################
# install targets
###############################################################################

install (
    TARGETS qwtinternal
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)

