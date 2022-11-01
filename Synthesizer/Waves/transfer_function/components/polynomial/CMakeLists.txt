cmake_minimum_required(VERSION 3.12)

add_library(polynomial OBJECT
    src/polynomial.cpp
)

target_include_directories(polynomial
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<TARGET_PROPERTY:eigen,INTERFACE_INCLUDE_DIRECTORIES>
)