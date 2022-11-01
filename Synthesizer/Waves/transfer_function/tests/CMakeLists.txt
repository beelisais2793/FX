################################
# GTest
################################
project(googletest-git NONE)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

################################
# Tests
################################
add_executable(${TEST_PROJECT_NAME}
    core_transfer_function_tests.cpp
    discretizer_tests.cpp
    frequency_analysis_tests.cpp
    polynomial_tests.cpp
    simulator_tests.cpp
    transfer_function_tests.cpp

    $<TARGET_OBJECTS:c2d>
    $<TARGET_OBJECTS:frequency_analysis>
    $<TARGET_OBJECTS:polynomial>
    $<TARGET_OBJECTS:simulation>
    $<TARGET_OBJECTS:tf>
    $<TARGET_OBJECTS:transfer_function>
)

target_link_libraries(${TEST_PROJECT_NAME}
    gtest 
    gtest_main
    
    c2d
    polynomial
    frequency_analysis
    simulation
    tf
    transfer_function
)