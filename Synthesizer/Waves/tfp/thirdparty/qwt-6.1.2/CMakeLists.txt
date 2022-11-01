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

set (QWT_VER_MAJ 6)
set (QWT_VER_MIN 1)
set (QWT_VER_PAT 2)
set (QWT_VERSION "${QWT_VER_MAJ}.${QWT_VER_MIN}.${QWT_VER_PAT}")

option (QWT_SHARED_LIB "Enable this to build Qwt as a shared library. Disable
to build as a static library" ON)

option (QWT_PLOT "Enables all classes that are needed to use the QwtPlot
widget" ON)

option (QWT_WIDGETS "Enables all classes that are needed to use all other
widgets (sliders, dials, etc.), besides QwtPlot" ON)

option (QWT_SVG "If you want to display svg images on the plot canvas or
export a plot to a svg document" ON)

option (QWT_OPENGL "If you want to use an OpenGL plot canvas" ON)

option (QWT_MATHML "You can use the MathML renderer of the Qt solutions
package to enable MathML support in Qwt. Because of license implications the (
modified ) code of the MML Widget solution is included and linked together
with the QwtMathMLTextEngine into an own lobrary." OFF)

option (QWT_DESIGNER "If you want to build the Qwt designer plugins." ON)

if (WIN32)
    option (QWT_DESIGNER_SELF_CONTAINED "Compile all Qwt classes into the
    designer plugin instead of linking it against the shared Qwt library. Has
    no effect when QWT_DESIGNER or QWT_SHARED_LIB are not both enabled." OFF)
endif ()

if (APPLE)
    option (QWT_FRAMEWORK "Build as a framework" ON)
endif ()

if (UNIX)
    option (QWT_PKG_CONFIG "Create and install pc files for pkg-config" OFF)
endif ()

message (STATUS "------------------------------------------------------------")
message (STATUS "Qwt settings")
message (STATUS " + Build as a shared library: ${QWT_SHARED_LIB}")
message (STATUS " + Enable QwtPlot: ${QWT_PLOT}")
message (STATUS " + Enable QwtWidgets: ${QWT_WIDGETS}")
message (STATUS " + Support for SVG: ${QWT_SVG}")
message (STATUS " + Use OpenGL for plot canvas: ${QWT_OPENGL}")
message (STATUS " + Use MathML for plot canvas: ${QWT_MATHML}")
message (STATUS " + Build Designer plugins: ${QWT_DESIGNER}")
if (WIN32)
    message (STATUS " + Self contained Designer plugins:
    ${QWT_DESIGNER_SELF_CONTAINED}")
endif ()
if (APPLE)
    message (STATUS " + Build as framework: ${QWT_FRAMEWORK}")
endif ()
if (UNIX)
    option (QWT_PKG_CONFIG "Create PC files for pkg-config:
    ${QWT_PKG_CONFIG}")
endif ()
message (STATUS "------------------------------------------------------------")

###############################################################################
# sub directories
###############################################################################

add_subdirectory ("src")
add_subdirectory ("textengines")
if (QWT_DESIGNER)
    add_subdirectory ("designer")
endif ()

