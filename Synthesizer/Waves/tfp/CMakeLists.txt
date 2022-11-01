cmake_minimum_required (VERSION 3.1.0)
cmake_policy (VERSION 3.0.0)
if (POLICY CMP0053)
    cmake_policy (SET CMP0053 OLD)
endif ()
if (POLICY CMP0054)
    cmake_policy (SET CMP0054 NEW) # don't dereference variables like "MSVC"
endif ()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()
message (STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(CMakeToolsHelpers OPTIONAL)
include (CheckCXXSourceCompiles)

###############################################################################
# Project name
###############################################################################

project ("transfer function plotter")

# Make macs happy
set (CMAKE_MACOSX_RPATH OFF)

# determine platform - this is used to replace @PLATFORM@ in the various
# export.h.in files.
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (PLATFORM "LINUX")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (PLATFORM "MACOSX")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (PLATFORM "WINDOWS")
endif ()

# Create vcproj.user file for Visual Studio to set debug working directory
# correctly.
# (Thanks David Avedissian)
function(create_vcproj_userfile TARGETNAME)
  if(MSVC)
    configure_file(
      "${CMAKE_SOURCE_DIR}/cmake/templates/VisualStudioUserFile.vcproj.user.in"
      "${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user"
      @ONLY)
    configure_file(
      "${CMAKE_SOURCE_DIR}/cmake/templates/VisualStudioUserFile.vcxproj.user.in"
      "${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user"
      @ONLY)
  endif()
endfunction()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/CMake/Modules")

###############################################################################
# set output locations
###############################################################################

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if (WIN32)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else ()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif ()

foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
endforeach (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

###############################################################################
# compiler definitions and flags
###############################################################################

set (CMAKE_CXX_STANDARD 11)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions (-DDEBUG)
    endif ()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions (-DDEBUG)
    endif ()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_WARNINGS)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        string (REGEX REPLACE "/MDd" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
        string (REGEX REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        add_definitions (/MTd)
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        string (REGEX REPLACE "/MD" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
        string (REGEX REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
        add_definitions (/MT)
    endif ()
endif ()

###############################################################################
# Options
###############################################################################

set (REAL_TYPE "double" CACHE STRING "The type to use for numerical computations")

# This is used by the other libraries to generate their export.h
set (TEMPLATE_EXPORT_H "${CMAKE_SOURCE_DIR}/CMake/Templates/export.hpp.in")

# complex hyperbolic functions
function (check_complex_hyperbolic SYMBOL RESULT_HAVE RESULT_SYMBOL)
    set (INCLUDES "#include <cmath>\n#include <complex>")

    # First, try to see if it is under std::
    check_cxx_source_compiles ("
        ${INCLUDES}\n
        int main() {
            std::complex<double> c(1, 4);
            std::complex<double> res = std::${SYMBOL}(c);
        }" ${SYMBOL}_RESULT)

    if (${SYMBOL}_RESULT)
        set (${RESULT_SYMBOL} "std::${SYMBOL}" PARENT_SCOPE)
        set (${RESULT_HAVE} 1 PARENT_SCOPE)
    else ()
        # kay, maybe it's global
        check_cxx_source_compiles ("
        ${INCLUDES}
        int main() {
            std::complex<double> c(1, 4);
            std::complex<double> res = ${SYMBOL}(c);
        }" ${SYMBOL}_RESULT)
        if (${SYMBOL}_RESULT)
            set (${RESULT_SYMBOL} "${SYMBOL}" PARENT_SCOPE)
            set (${RESULT_HAVE} 1 PARENT_SCOPE)
        else ()
            set (${RESULT_SYMBOL} "tfp::${SYMBOL}" PARENT_SCOPE)
            set (${RESULT_HAVE} 0 PARENT_SCOPE)
        endif ()
    endif ()
endfunction ()

check_complex_hyperbolic ("sinh" HAVE_SINH SINH_SYMBOL)
check_complex_hyperbolic ("cosh" HAVE_COSH COSH_SYMBOL)
check_complex_hyperbolic ("tanh" HAVE_TANH TANH_SYMBOL)
check_complex_hyperbolic ("asinh" HAVE_ASINH ASINH_SYMBOL)
check_complex_hyperbolic ("acosh" HAVE_ACOSH ACOSH_SYMBOL)
check_complex_hyperbolic ("atanh" HAVE_ATANH ATANH_SYMBOL)

option (TFP_BUILD_TESTS "Whether to build unit tests" ON)

message (STATUS "------------------------------------------------------------")
message (STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message (STATUS "Platform is ${PLATFORM}")
message (STATUS "Real type: ${REAL_TYPE}")
message (STATUS "Build tests: ${TFP_BUILD_TESTS}")
message (STATUS "------------------------------------------------------------")

include_directories ("${CMAKE_CURRENT_LIST_DIR}/tfp/include")
include_directories (${CMAKE_BINARY_DIR})                    # need this because MOC outputs to binary_dir/tfp
include_directories ("${CMAKE_BINARY_DIR}/tfp/include")      # need this for config.hpp and export.hpp

if (TFP_BUILD_TESTS)
    include_directories ("${CMAKE_SOURCE_DIR}/thirdparty/googlemock/include")
    include_directories ("${CMAKE_SOURCE_DIR}/thirdparty/googletest/include")
endif ()

# must be in this order due to linking
set (EIGEN_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/thirdparty/eigen-3.3.4")
add_subdirectory ("thirdparty/kiss_fft130")
add_subdirectory ("thirdparty/pugixml-1.7")
add_subdirectory ("thirdparty/qwt-6.1.2")
add_subdirectory ("thirdparty/qwtplot3d-0.3.1")
add_subdirectory ("thirdparty/ads-2.0.1")
add_subdirectory ("tfp")
add_subdirectory ("application")
if (TFP_BUILD_TESTS)
    add_subdirectory ("thirdparty/googlemock")
endif ()

