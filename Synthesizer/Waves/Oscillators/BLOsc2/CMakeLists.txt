cmake_minimum_required (VERSION 2.8)
project (BLOsc2)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/external_libraries/libsndfile/)

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")



add_library(BLOsc2 MODULE BLOsc2.cpp)
