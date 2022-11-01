cmake_minimum_required(VERSION 2.8)
project(Gamma)

message(STATUS "==== Configuring Gamma.")

# C++11 is required for Gamma
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if ((${CMAKE_SYSTEM_NAME} MATCHES "Darwin") OR (${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
  if(NOT (COMPILER_SUPPORTS_CXX11 OR COMPILER_SUPPORTS_CXX0X))
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif()
endif()

if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
  if(USE_LIB_CPP)
    message(STATUS "Using libc++.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif(USE_LIB_CPP)
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()


if(NOT DEFINED BUILD_ROOT_DIR)
  # not current source dir so that it builds at allolib dir
  set(BUILD_ROOT_DIR "${CMAKE_SOURCE_DIR}")
endif(NOT DEFINED BUILD_ROOT_DIR)

# Gamma Library
set(GAMMA_SRC
  src/arr.cpp
  # src/AudioIO.cpp
  src/Conversion.cpp
  src/DFT.cpp
  src/Domain.cpp
  src/FFT_fftpack.cpp
  src/fftpack++1.cpp
  src/fftpack++2.cpp
  src/Print.cpp
  src/scl.cpp
  src/Recorder.cpp
  src/Scheduler.cpp
  # src/SoundFile.cpp
  src/Timer.cpp
  src/fftpack++.h)

# include_directories(${CMAKE_CURRENT_LIST_DIR})
file(GLOB GAMMA_INSTALL_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} Gamma/*.h)
# Copy headers to build directory (only if header has changed)
# foreach(header ${GAMMA_INSTALL_HEADERS})
# 	execute_process(COMMAND ${CMAKE_COMMAND} -E
# 					copy_if_different "${header}" "${BUILD_ROOT_DIR}/build/include/${header}"
# 					WORKING_DIRECTORY  "${CMAKE_CURRENT_LIST_DIR}")
# endforeach()

# Dependencies

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
# find_package(Portaudio REQUIRED)
find_package(LibSndFile QUIET)
find_package(Threads REQUIRED)
# message(STATUS "Portaudio: ${PORTAUDIO_LIBRARY}  ${PORTAUDIO_DEFINITIONS}")
message(STATUS "libsndfile: ${SNDFILE_LIBRARY}")
message(STATUS "threading: ${CMAKE_THREAD_LIBS_INIT}")

if (SNDFILE_FOUND)
  # message("sndfile found")
  list(APPEND GAMMA_SRC ${CMAKE_CURRENT_LIST_DIR}/src/SoundFile.cpp)
else()
  # message("sndfile not found")
  list(REMOVE_ITEM GAMMA_INSTALL_HEADERS Gamma/SoundFile.h)
endif()

add_library(Gamma STATIC ${GAMMA_SRC} ${GAMMA_INSTALL_HEADERS})
# set(LIBRARY_OUTPUT_PATH "${BUILD_ROOT_DIR}/build/lib")
# set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/bin")
set_target_properties(Gamma PROPERTIES
    DEBUG_POSTFIX d
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/lib
)
# set_target_properties(Gamma PROPERTIES DEBUG_POSTFIX _debug)

# TODO: allow choosing static or dynamic libraries

# include_directories(${PORTAUDIO_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS})

# if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Needed by portaudio
#   find_library(AUDIOUNIT_FM AudioUnit)
#   find_library(COREAUDIO_FM CoreAudio)
#   find_library(CORESERVICES_FM CoreServices)
#   find_library(AUDIOTOOLBOX_FM AudioToolbox)
#   set(PA_FRAMEWORKS ${AUDIOUNIT_FM} ${COREAUDIO_FM} ${CORESERVICES_FM} ${AUDIOTOOLBOX_FM})
# endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  #Needed to use Gamma with certain frameworks, e.g. JUCE
  set_target_properties(Gamma PROPERTIES COMPILE_FLAGS "-fPIC")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# set_target_properties(Gamma
#   PROPERTIES
#   GAMMA_LINK_LIBRARIES "${GAMMA_LINK_LIBRARIES}"
#   )
# set_target_properties(Gamma
#   PROPERTIES
#   GAMMA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}
#   )

target_link_libraries(Gamma PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(Gamma PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
  "$<INSTALL_INTERFACE:include>"
)


if (SNDFILE_FOUND)
  # message("sndfile found")
  target_link_libraries(Gamma PUBLIC ${SNDFILE_LIBRARIES})
  target_include_directories(Gamma PUBLIC ${SNDFILE_INCLUDE_DIRS})
else()
  # message("sndfile not found")
endif()

# Run from path
# if((GAMMA_BUILD_DIR) OR (GAMMA_BUILD_APP_FILE))
#     set(BUILD_DIR ${GAMMA_BUILD_DIR})
#     set(BUILD_APP_FILE ${GAMMA_BUILD_APP_FILE})
#     include(${CMAKE_CURRENT_LIST_DIR}/cmake_modules/CMakeRunTargets.cmake)
#     set(BUILD_EXAMPLES 0)
# endif((GAMMA_BUILD_DIR) OR (GAMMA_BUILD_APP_FILE))

# Examples -----------------------------------------------

# if(BUILD_EXAMPLES)
#   set(EXAMPLE_DIRS algorithmic analysis curves effects envelope filter function io oscillator source spatial spectral synthesis synths techniques)
#   foreach(dir ${EXAMPLE_DIRS})
# 	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin/examples/${dir}")
# 	file(GLOB EXAMPLE_FILES RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "examples/${dir}/*.cpp")
#     foreach(example ${EXAMPLE_FILES})
#       get_filename_component(EXAMPLE_NAME ${example} NAME_WE) # Get name w/o extension
#       if (ALLOSYSTEM_BUILD)
# 	set(EXAMPLE_TARGET gamma_examples_${dir}_${EXAMPLE_NAME})
#       else(ALLOSYSTEM_BUILD)
# 	set(EXAMPLE_TARGET examples_${dir}_${EXAMPLE_NAME})
#       endif(ALLOSYSTEM_BUILD)
#       add_executable(${EXAMPLE_TARGET} ${example})
#       target_link_libraries("${EXAMPLE_TARGET}" Gamma ${PORTAUDIO_LIBRARIES} ${SNDFILE_LIBRARIES} ${PA_FRAMEWORKS})
#       add_custom_target("${EXAMPLE_TARGET}_run"
# 	COMMAND "${EXAMPLE_TARGET}"
# 	DEPENDS "${EXAMPLE_TARGET}"
# 	WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
# 	SOURCES ${example}
# 	COMMENT "Running: ${EXAMPLE_TARGET}")
#     endforeach(example)
#   endforeach(dir)
# endif(BUILD_EXAMPLES)

# Unit tests
if (GAMMA_BUILD_TESTS)
  enable_testing()

  add_executable(gammaUnitTests tests/unitTests.cpp)
  target_include_directories(gammaUnitTests PRIVATE ${CMAKE_CURRENT_LIST_DIR})
  target_link_libraries(gammaUnitTests Gamma ${GAMMA_LINK_LIBRARIES})
  add_test(NAME gammaUnitTests
  	 COMMAND $<TARGET_FILE:gammaUnitTests> ${TEST_ARGS})
endif()

install(TARGETS Gamma
  EXPORT Gamma-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(EXPORT Gamma-targets DESTINATION lib/cmake/Gamma)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Gamma DESTINATION include)
