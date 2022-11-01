cmake_minimum_required(VERSION 3.21)
project(modfm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(ABSL_PROPAGATE_CXX_STD ON)

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

# Third party packages are pulled in via FetchContent
include(FetchContent)

# Glog
FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
)
FetchContent_MakeAvailable(glog)

# Abseil
FetchContent_Declare(
        abseil
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
)
FetchContent_MakeAvailable(abseil)

# Sigslot
FetchContent_Declare(
        sigslot
        GIT_REPOSITORY https://github.com/palacaze/sigslot.git
)
FetchContent_MakeAvailable(sigslot)

add_library(
        modfmlib
        STATIC
        src/oscillator.cc
        src/player.cc
        src/patch.cc
        src/envgen.cc)
target_include_directories(modfmlib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/modfm>
        $<INSTALL_INTERFACE:include/modfm>  # <prefix>/include/modfm
)

target_link_libraries(modfmlib PUBLIC
        Pal::Sigslot
        absl::statusor
        glog::glog)

install(TARGETS modfmlib
        EXPORT modfmlibTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

option(MODFM_BUILD_UI "Build example UI" OFF)
if (MODFM_BUILD_UI)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)

    # Gflags
    FetchContent_Declare(
            gflags
            GIT_REPOSITORY https://github.com/gflags/gflags.git
    )
    FetchContent_MakeAvailable(gflags)

    # PortAudio
    FetchContent_Declare(
            portaudio
            GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    )
    if(NOT portaudio_POPULATED)
        FetchContent_Populate(portaudio)
        add_subdirectory(${portaudio_SOURCE_DIR})
        include_directories(${portaudio_SOURCE_DIR}/include)
        include_directories(${portaudio_SOURCE_DIR}/include)
    endif()

    # PortMidi
    FetchContent_Declare(
            portmidi
            GIT_REPOSITORY https://github.com/PortMidi/portmidi.git
    )
    if(NOT portmidi_POPULATED)
        FetchContent_Populate(portmidi)
        add_subdirectory(${portmidi_SOURCE_DIR})
        include_directories(${portmidi_SOURCE_DIR}/pm_common)
        include_directories(${portmidi_SOURCE_DIR}/porttime)
    endif()

    # Nanogui
    option(NANOGUI_BUILD_PYTHON "" OFF)
    FetchContent_Declare(
            nanogui
            GIT_REPOSITORY https://github.com/mitsuba-renderer/nanogui.git
    )
    FetchContent_MakeAvailable(nanogui)

    add_executable(modfm
            src/ui/main.cc
            src/ui/patch_editor.h
            src/ui/patch_editor.cc
            src/ui/drawbars.h
            src/ui/drawbars.cc
            src/ui/device_selector.h
            src/ui/device_selector.cc
            src/ui/gui.h
            src/ui/gui.cc
            src/ui/midi.h
            src/ui/midi.cc
            src/envgen.cc)

    set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

    target_include_directories(modfm PRIVATE
            src/
            )
    if (WIN32)
        set(PLATFORM_LIBRARIES})
    else ()
        set(PLATFORM_LIBRARIES pthread rt GL glfw tbb)
    endif ()

    target_link_libraries(modfm
            modfmlib
            absl::utility
            absl::statusor
            ${PLATFORM_LIBRARIES}
            glog::glog
            gflags
            glfw
            pulse-simple
            portmidi
            PortAudio
            Pal::Sigslot
            nanogui)
endif ()