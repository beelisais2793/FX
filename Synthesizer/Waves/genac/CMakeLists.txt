cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Application names
set(GENAC "genac")

project(${GENAC})

# Enable c, and c++ language
enable_language(C CXX)

# Application descriptions
set(GENAC_DESCRIPTION
  "[GEN]erates and symbolically [A]nalyzes analog [C]ircuits using GiNaC")

# Library names
set(ANALYZERLIB "analyzer")
set(SYNTHESIZERLIB "synthesizer")

# Project version
set(GENAC_VERSION_MAJOR 0)
set(GENAC_VERSION_MINOR 1)
set(GENAC_VERSION_PATCH 0)
set(GENAC_VERSION
  "${ANALYZER_VERSION_MAJOR}.${ANALYZER_VERSION_MINOR}.${ANALYZER_VERSION_PATCH}")

# Location of additional cmake modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(OutOfSourceConfig)
include(BuildTypeConfig)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS
    "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

option(BUILD_GENAC "Build synthesizer" ON)
option(BUILD_MAN "Build manuals" OFF)
option(BUILD_DOC "Build documents" OFF)
option(BUILD_TEST "Build tests" OFF)

if(BUILD_GENAC)
  add_subdirectory(lib)
  add_subdirectory(src)
endif()

if(BUILD_TEST)
  add_subdirectory(test)
endif()

if(BUILD_MAN)
  add_subdirectory(man)
endif()

if(BUILD_DOC)
  add_subdirectory(doc)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/samples/
        DESTINATION /usr/share/genac/samples)
