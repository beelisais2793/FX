cmake_minimum_required(VERSION 3.13.0)
project(fast-additive-comparison VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(xsimd REQUIRED) 
find_package(nanobench REQUIRED) 

get_target_property(xsimd_INCLUDE_DIRS xsimd INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(nanobench_INCLUDE_DIRS nanobench::nanobench INTERFACE_INCLUDE_DIRECTORIES)


add_executable(compare-accuracy src/comparisons/compare-accuracy.cpp)
add_executable(compare-speed src/comparisons/compare-speed.cpp)

target_compile_options(compare-accuracy PUBLIC /W2 /O2 /arch:AVX2 /fp:fast /EHsc /permissive-)
target_compile_options(compare-speed PUBLIC /W2 /O2 /arch:AVX2 /fp:fast /EHsc /permissive-)

target_include_directories(compare-accuracy PUBLIC ${xsimd_INCLUDE_DIRS})
target_include_directories(compare-speed PUBLIC ${nanobench_INCLUDE_DIRS} ${xsimd_INCLUDE_DIRS})

target_link_libraries(compare-speed PRIVATE nanobench::nanobench)

set_target_properties(compare-accuracy PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO 
) 
set_target_properties(compare-speed PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO 
) 


 
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
