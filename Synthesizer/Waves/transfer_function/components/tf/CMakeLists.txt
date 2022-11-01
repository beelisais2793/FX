cmake_minimum_required(VERSION 3.12)

add_library(tf OBJECT
    src/core_transfer_function.cpp
)

target_include_directories(tf
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<TARGET_PROPERTY:c2d,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:polynomial,INTERFACE_INCLUDE_DIRECTORIES>
)