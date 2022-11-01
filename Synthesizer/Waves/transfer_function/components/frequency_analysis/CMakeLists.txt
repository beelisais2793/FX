cmake_minimum_required(VERSION 3.12)

add_library(frequency_analysis OBJECT
    src/complex.cpp
    src/complex_transfer_function.cpp
)

target_include_directories(frequency_analysis
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<TARGET_PROPERTY:polynomial,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:tf,INTERFACE_INCLUDE_DIRECTORIES>
)