###############################################################################
# compiler flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wstrict-prototypes -Wmissing-prototypes
            -Waggregate-return -Wcast-align -Wcast-qual -Wnested-externs
            -Wshadow -Wbad-function-cast -Wwrite-strings)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wstrict-prototypes -Wmissing-prototypes
            -Waggregate-return -Wcast-align -Wcast-qual -Wnested-externs
            -Wshadow -Wbad-function-cast -Wwrite-strings)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# config
###############################################################################

# Required by export.hpp.in and config.hpp.in
set (LIBRARY_NAME KISS_FFT)
set (BUILD_TYPE SHARED)

set (KISS_FFT_DATATYPE "double" CACHE STRING "The datatype to use for all transforms. Can be 'float', 'int32_t', 'int16_t', 'simd', etc.")
set (KISS_FFT_USE_SIMD "0")
if (${KISS_FFT_DATATYPE} STREQUAL "int32_t")
    set (KISS_FFT_FIXED_POINT "32")
elseif (${KISS_FFT_DATATYPE} STREQUAL "int16_t")
    set (KISS_FFT_FIXED_POINT "16")
elseif (${KISS_FFT_DATATYPE} STREQUAL "simd")
    set (KISS_FFT_USE_SIMD "1")
    add_definitions (-msse)
endif ()
set (kiss_fft_scalar ${KISS_FFT_DATATYPE})

if (${PLATFORM} MATCHES "WINDOWS")
	option (KISS_FFT_USE_ALLOCA "Use alloca() instead of malloc()" OFF)
else ()
	option (KISS_FFT_USE_ALLOCA "Use alloca() instead of malloc()" ON)
endif ()
if (KISS_FFT_USE_ALLOCA)
    set (KISS_FFT_USE_ALLOCA "1")
endif ()

configure_file (${TEMPLATE_EXPORT_H} "include/kiss_fft/export.hpp")
configure_file ("include/kiss_fft/config.h.in" "include/kiss_fft/config.h")

option (KISS_FFT "Build FFT library" ON)
option (KISS_FASTCONV "Build fast convolution library" OFF)
option (KISS_FASTCONVR "" OFF)
option (KISS_PSDPNG "" OFF)
option (KISS_DUMPHDR "" OFF)

###############################################################################
# source files and library definition
###############################################################################

set (KISS_FFT_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    CACHE STRING "kiss_fft include directories"
)

include_directories (${KISS_FFT_INCLUDE_DIRS})

add_definitions (-DKISS_FFT_BUILDING)

if (KISS_FFT)
    add_library (kiss_fft "src/kiss_fft.c" "src/kiss_fftr.c")
    create_vcproj_userfile (kiss_fft)
endif ()


###############################################################################
# install targets
###############################################################################

if (INSTALL_LIBS)
    install (
        TARGETS kiss_fft
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin"
    )
    install (DIRECTORY "include" DESTINATION ".")
	install (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include" DESTINATION ".")
endif ()
