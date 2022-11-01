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

set (LIBRARY_NAME ADS)
if (ADS_SHARED_LIB)
    set (BUILD_TYPE SHARED)
else ()
    set (BUILD_TYPE STATIC)
endif ()
configure_file (${TEMPLATE_EXPORT_H} "include/ads/export.h")

###############################################################################
# Defines
###############################################################################

if (ADS_SHARED_LIB)
    add_definitions (-DADS_EXPORT)
    add_definitions (-DADS_BUILDING)
endif ()

###############################################################################
# Library, source and header files
###############################################################################

set (ADS_HEADERS
    "include/ads/globals.h"
    "include/ads/DockAreaWidget.h"
    "include/ads/DockContainerWidget.h"
    "include/ads/DockManager.h"
    "include/ads/DockWidget.h"
    "include/ads/DockWidgetTitleBar.h"
    "include/ads/FloatingDockContainer.h"
    "include/ads/DockOverlay.h"
    "include/ads/DockSplitter.h")
set (ADS_SOURCES
    "src/globals.cpp"
    "src/DockAreaWidget.cpp"
    "src/DockContainerWidget.cpp"
    "src/DockManager.cpp"
    "src/DockWidget.cpp"
    "src/DockWidgetTitleBar.cpp"
    "src/FloatingDockContainer.cpp"
    "src/DockOverlay.cpp"
    "src/DockSplitter.cpp")
set (ADS_MOC_HEADERS
    "include/ads/DockAreaWidget.h"
    "include/ads/DockContainerWidget.h"
    "include/ads/DockManager.h"
    "include/ads/DockWidget.h"
    "include/ads/DockWidgetTitleBar.h"
    "include/ads/FloatingDockContainer.h"
    "include/ads/DockOverlay.h"
    "include/ads/DockSplitter.h")
set (ADS_RESOURCES
    "res/ads.qrc")
set (ADS_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include")

find_package (Qt5 COMPONENTS Core Gui Widgets REQUIRED)
qt5_wrap_cpp (ADS_MOC_SOURCES ${ADS_MOC_HEADERS})
qt5_add_resources (ADS_RESOURCES_GEN ${ADS_RESOURCES})

unset (ADS_INCLUDE_DIRS CACHE)
set (ADS_INCLUDE_DIRS ${ADS_INCLUDE_DIRS} CACHE STRING "ADS include directories")
include_directories (${ADS_INCLUDE_DIRS})

add_library (ads ${BUILD_TYPE}
    ${ADS_HEADERS}
    ${ADS_SOURCES}
    ${ADS_MOC_SOURCES}
    ${ADS_RESOURCES_GEN}
)
unset (ADS_LIBRARIES CACHE)
set (ADS_LIBRARIES ads CACHE STRING "ADS libraries")

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (ads)

target_link_libraries (ads Qt5::Core Qt5::Gui Qt5::Widgets)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS ads
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)
