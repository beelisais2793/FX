###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-long-long -Wno-variadic-macros -Wno-missing-field-initializers -Wno-write-strings)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-long-long -Wno-variadic-macros -Wno-missing-field-initializers -Wno-write-strings)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

add_definitions (-D_USE_MATH_DEFINES)

###############################################################################
# source files and library definition
###############################################################################

add_executable (tfp_application
    "src/main.cpp"
)

# generates the correct project files for visual studio, setting thee correct
# debug working directory and other stuff
create_vcproj_userfile (tfp_application)

###############################################################################
# dependencies
###############################################################################

# must be in this order due to linking
include_directories (${KISS_FFT_INCLUDE_DIRS})
include_directories (${QWT_INCLUDE_DIRS})
include_directories (${QWTPLOT3D_INCLUDE_DIRS})
include_directories (${EIGEN_INCLUDE_DIRS})
include_directories (${PUGIXML_INCLUDE_DIRS})

target_link_libraries (tfp_application tfp)

###############################################################################
# plugins
###############################################################################

add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/bode-plot")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/butterworth-filter")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/chebyshev-filter")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/complex-plane-3d")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/dpsfg")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/elliptic-filter")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/pole-zero-plot")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/std-low-order-filters")
add_subdirectory ("${CMAKE_CURRENT_LIST_DIR}/plugins/step-impulse-response")
