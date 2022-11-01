cmake_minimum_required(VERSION 3.12)

add_library(simulation OBJECT
    src/discrete_simulator.cpp
)

target_include_directories(simulation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<TARGET_PROPERTY:tf,INTERFACE_INCLUDE_DIRECTORIES>
)