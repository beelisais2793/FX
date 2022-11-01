###############################################################################
# Library, source and header files
###############################################################################

set (ADS_DEMO_HEADERS
    "mainwindow.h")
set (ADS_DEMO_SOURCES
    "mainwindow.cpp"
    "main.cpp")
set (ADS_DEMO_MOC_HEADERS
    "mainwindow.h")
set (ADS_DEMO_FORMS
    "mainwindow.ui")

find_package (Qt5 COMPONENTS Core Widgets Gui REQUIRED)

qt5_wrap_cpp (ADS_DEMO_MOC_SOURCES ${ADS_DEMO_MOC_HEADERS})
qt5_wrap_ui (ADS_DEMO_FORMS_MOC ${ADS_DEMO_FORMS})

include_directories (${ADS_INCLUDE_DIRS})
include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_executable (ads_demo
    ${ADS_DEMO_HEADERS}
    ${ADS_DEMO_SOURCES}
    ${ADS_DEMO_MOC_SOURCES}
    ${ADS_DEMO_FORMS_MOC}
)

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (ads_demo)

target_link_libraries (ads_demo ${ADS_LIBRARIES})

###############################################################################
# install targets
###############################################################################

install (
    TARGETS ads_demo
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)

