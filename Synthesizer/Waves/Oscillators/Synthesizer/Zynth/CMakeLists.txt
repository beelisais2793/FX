message(STATUS "Checking Include Path" $ENV{CMAKE_INCLUDE_PATH} ${CMAKE_INCLUDE_PATH})
message(STATUS "Checking Library Path" $ENV{CMAKE_LIBRARY_PATH} ${CMAKE_LIBRARY_PATH})
message(STATUS "Building on a '${CMAKE_SYSTEM_NAME} v${CMAKE_SYSTEM_VERSION}' System and an '${CMAKE_SYSTEM_PROCESSOR}' CPU")

find_package(FFTW REQUIRED)
find_package(MXML REQUIRED)
find_package(PTHREAD REQUIRED)
find_package(ZLIB REQUIRED)

link_directories(
    ${FFTW_LIBRARY_DIRS}
    ${MXML_LIBRARY_DIRS}
    ${PTHREAD_LIBRARY_DIRS}
    ${ZLIB_LIBRARY_DIRS}
    )

include_directories(
    ${FFTW_INCLUDE_DIR}
    ${MXML_INCLUDE_DIR}
    ${PTHREAD_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_definitions(-DVERSION="${VERSION}")
add_definitions(-DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(MSVC)
    if(CMAKE_CL_64)
        add_definitions(-D_AMD64_)
    endif(CMAKE_CL_64)
    add_definitions(-D__WINDOWS_MM__)
    set(OS_LIBRARIES "winmm.lib")
else(MSVC)
    option(BuildForAMD_X86_64 "Build for AMD x86_64 system" OFF)
    option(BuildForCore2_X86_64 "Build for Intel Core2 x86_64 system" OFF)
    option(BuildForDebug "Include gdb debugging support" OFF)

    set(CMAKE_BUILD_TYPE "Release")

    set(BuildOptions_x86_64AMD
        "-O3 -march=athlon64 -m64 -Wall -ffast-math -fno-finite-math-only -fomit-frame-pointer"
        CACHE STRING "X86_64 compiler options"
    )

    set(BuildOptions_X86_64Core2
        "-O3 -march=core2 -m64 -Wall -ffast-math -fno-finite-math-only -fomit-frame-pointer"
        CACHE STRING "X86_64 compiler options"
    )

    set(BuildOptionsBasic
        "-O3 -msse -msse2 -mfpmath=sse -ffast-math -fomit-frame-pointer"
        CACHE STRING "basic X86 complier options"
    )

    set(BuildOptionsDebug
        "-O0 -g3 -ggdb -Wall -Wpointer-arith" CACHE STRING "Debug build flags"
    )

    add_definitions(
         -g #TODO #todo put in a better location
         -Wall
         -Wextra
         )
    if(NOT AVOID_ASM)
        message(STATUS "Compiling with x86 opcode support")
        add_definitions(-DASM_F2I_YES)
    endif()

    if(BuildForDebug)
        set(CMAKE_BUILD_TYPE "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG ${BuildOptionsDebug})
        message(STATUS "Building for ${CMAKE_BUILD_TYPE}, flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    else(BuildForDebug)
        set(CMAKE_BUILD_TYPE "Release")
        if(BuildForAMD_X86_64)
            set(CMAKE_CXX_FLAGS_RELEASE ${BuildOptions_x86_64AMD})
        else(BuildForAMD_X86_64)
            if(BuildForCore2_X86_64)
                set(CMAKE_CXX_FLAGS_RELEASE ${BuildOptions_X86_64Core2})
            else(BuildForCore2_X86_64)
                set(CMAKE_CXX_FLAGS_RELEASE ${BuildOptionsBasic})
            endif(BuildForCore2_X86_64)
        endif(BuildForAMD_X86_64)
        message(STATUS "Building for ${CMAKE_BUILD_TYPE}, flags: ${CMAKE_CXX_FLAGS_RELEASE}")
    endif(BuildForDebug)
    add_definitions(-fPIC)
endif(MSVC)

#####################################################################################
### BEGIN : Core libZynth code
#   - needed for all executables and libraries in this cmakelists
add_subdirectory(Misc)
add_subdirectory(Synth)
add_subdirectory(Effects)
add_subdirectory(Params)
add_subdirectory(DSP)
add_subdirectory(Nio)

set(DEFAULT_IN "NULL")
set(DEFAULT_OUT "NULL")

find_package(Alsa)
find_package(JACK)
find_package(PORTAUDIO)

set(AlsaEnable ${ALSA_FOUND} CACHE BOOL
    "Enable support for Advanced Linux Sound Architecture")
set(JackEnable ${JACK_FOUND} CACHE BOOL
    "Enable support for JACK Audio Connection toolKit")
set(PaEnable ${PORTAUDIO_FOUND} CACHE BOOL
    "Enable support for Port Audio System")
set(LashEnable ${LASH_FOUND} CACHE BOOL
    "Enable LASH Audio Session Handler")

if(AlsaEnable)
    list(APPEND nio_LIBRARIES ${ASOUND_LIBRARY})
    list(APPEND nio_LIBRARY_DIRS ${ASOUND_LIBRARY_DIRS})
    list(APPEND libzynth_nio_SRCS Nio/AlsaEngine.cpp)
    add_definitions(-DALSA=1)
    message(STATUS "Compiling with alsa")
endif(AlsaEnable)

if(JackEnable)
    include_directories(${JACK_INCLUDE_DIR})
    list(APPEND nio_LIBRARIES ${JACK_LIBRARIES})
    list(APPEND nio_LIBRARY_DIRS ${JACK_LIBRARY_DIRS})
    list(APPEND libzynth_nio_SRCS Nio/JackEngine.cpp)
    add_definitions(-DJACK=1)
    message(STATUS "Compiling with jack")
endif(JackEnable)

if(PaEnable)
    include_directories(${PORTAUDIO_INCLUDE_DIR})
    list(APPEND nio_LIBRARIES ${PORTAUDIO_LIBRARIES})
    list(APPEND nio_LIBRARY_DIRS ${PORTAUDIO_LIBRARY_DIRS})
    list(APPEND libzynth_nio_SRCS Nio/PaEngine.cpp)
    add_definitions(-DPORTAUDIO=1)
    set(DEFAULT_IN "NULL")
    set(DEFAULT_OUT "PA")
    message(STATUS "Compiling with portaudio")
endif(PaEnable)

if(LashEnable)
    include_directories(${LASH_INCLUDE_DIRS})
    list(APPEND nio_LIBRARIES ${LASH_LIBRARIES})
    list(APPEND nio_LIBRARY_DIRS ${LASH_LIBRARY_DIRS})
    add_definitions(-DLASH=1)
    message(STATUS "Compiling with lash")
endif(LashEnable)

add_definitions(-DIN_DEFAULT="${DEFAULT_IN}")
add_definitions(-DOUT_DEFAULT="${DEFAULT_OUT}")

add_library(libzynth STATIC
    ${libzynth_nio_SRCS}
    ${libzynth_misc_SRCS}
    ${libzynth_dsp_SRCS}
    ${libzynth_effect_SRCS}
    ${libzynth_params_SRCS}
    ${libzynth_synth_SRCS}
    globals.h
    )

target_link_libraries(libzynth
    ${nio_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${MXML_LIBRARIES}
    ${PTHREAD_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${OS_LIBRARIES}
    )


### END : Core libZynth code
#####################################################################################
