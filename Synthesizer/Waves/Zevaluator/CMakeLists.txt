cmake_minimum_required(VERSION 3.16)

project(Zeval VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Set up our project
add_library(${PROJECT_NAME} INTERFACE)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Set up Catch2 for unit tests
find_package(Catch2 REQUIRED)

add_executable(zeval_test test/test.cpp)

target_link_libraries(zeval_test PRIVATE Catch2::Catch2)

target_include_directories(zeval_test PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(CTest)
include(Catch)

catch_discover_tests(zeval_test)

enable_testing()
