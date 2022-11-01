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

option (ADS_SHARED_LIB "Enable this to build ADS as a shared library. Disable to build as a static library" ON)
option (ADS_DEMO "Build the demo application" OFF)

message (STATUS "------------------------------------------------------------")
message (STATUS "ADS settings")
message (STATUS " + Build as a shared library: ${ADS_SHARED_LIB}")
message (STATUS " + Build demo: ${ADS_DEMO}")
message (STATUS "------------------------------------------------------------")

###############################################################################
# sub directories
###############################################################################

add_subdirectory ("ads")
if (ADS_DEMO)
    add_subdirectory ("demo")
endif ()