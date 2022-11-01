cmake_minimum_required(VERSION 3.12)

add_library(c2d OBJECT
    src/polynomial_discretizer.cpp
    src/polynomial_fraction.cpp
    src/transfer_function_discretizer.cpp
)

target_include_directories(c2d
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<TARGET_PROPERTY:polynomial,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:tf,INTERFACE_INCLUDE_DIRECTORIES>
)