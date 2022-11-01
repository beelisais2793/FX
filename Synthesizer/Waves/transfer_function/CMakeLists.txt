###############################################
################# Cmake Setup #################
###############################################
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME transfer_function)
set(TEST_PROJECT_NAME ${PROJECT_NAME}_tests)

project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEFAULT_BUILD_TYPE "Debug")

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -g -O0 -fprofile-arcs -ftest-coverage")
endif()
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

option(BUILD_TESTS "Build tests" ON)
option(MAKE_CPPCHECK "Make static code analysis" OFF)
option(MAKE_CPPLINT "Make code style analysis" OFF)

################################################
########### Add sources and includes ###########
################################################
include(GNUInstallDirs)

add_library(${PROJECT_NAME} 
    src/transfer_function.cpp

    $<TARGET_OBJECTS:c2d>
    $<TARGET_OBJECTS:frequency_analysis>
    $<TARGET_OBJECTS:polynomial>
    $<TARGET_OBJECTS:simulation>
    $<TARGET_OBJECTS:tf>
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
    PRIVATE
        $<TARGET_PROPERTY:c2d,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:frequency_analysis,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:polynomial,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:simulation,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:tf,INTERFACE_INCLUDE_DIRECTORIES>
)

add_subdirectory(components)

###############################################
############### Static Analysis ###############
###############################################
include(cppcheck)
include(cpplint)

###############################################
############### CodeCoverage ##################
###############################################
if(UNIX)
    if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
        include(code_coverage)

        SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
        
        setup_target_for_coverage(
            coverage
            ${TEST_PROJECT_NAME}
            ${PROJECT_NAME}_coverage_report
        ) 
    endif() # CMAKE_BUILD_TYPE STREQUAL "Coverage"
endif() # UNIX

###############################################
################ Unit Tests ###################
###############################################
if(BUILD_TESTS)    
    project(${TEST_PROJECT_NAME})

    add_subdirectory(tests)    
    enable_testing()
    add_test(${TEST_PROJECT_NAME} ${TEST_PROJECT_NAME})    
endif() #   BUILD_TESTS
