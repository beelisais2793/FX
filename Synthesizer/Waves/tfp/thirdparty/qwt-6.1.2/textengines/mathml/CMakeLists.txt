###############################################################################
# Library, source and header files
###############################################################################

file (GLOB QWT_MATHML_HEADERS
    "qwt_mathml_text_engine.h"
    "qwt_mml_document.h")
file (GLOB QWT_MATHML_SOURCES
    "qwt_mathml_text_engine.cpp"
    "qwt_mml_document.cpp")
set (QWT_MATHML_MOC_HEADERS "")
set (QWT_MATHML_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}")

#qt5_wrap_cpp (QWT_MATHML_MOC_SOURCES ${QWT_MATHML_MOC_HEADERS})

set (LIBRARY_NAME QWT_MATHML)
if (QWT_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()

unset (QWT_MATHML_INCLUDE_DIRS CACHE)
set (QWT_MATHML_INCLUDE_DIRS ${QWT_MATHML_INCLUDE_DIRS} CACHE STRING "Qwt MathML include directories")
include_directories (${QWT_MATHML_INCLUDE_DIRS})
include_directories (${QWT_INCLUDE_DIRS})

add_library (qwtmathml ${BUILD_TYPE}
    ${QWT_MATHML_SOURCES}
)

unset (QWT_MATHML_LIBRARIES CACHE)
set (QWT_MATHML_LIBRARIES qwtmathml CACHE STRING "Qwt MathML libraries")

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (qwtmathml)

add_definitions (-DQWT_BUILDING)

target_link_libraries (qwtmathml Qt5::Core Qt5::Widgets Qt5::Xml)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS qwtmathml
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)
