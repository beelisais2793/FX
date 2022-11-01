cmake_minimum_required (VERSION 3.1)
project (GPOsc)


# dependencies
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/")

# source 
set(INCLUDE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GPModel.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GPVarianceKernel.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GPRandom.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GPDrunkRNG.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/GPContinuousOsc.h"	)

set(SOURCE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/GPModel.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/GPVarianceKernel.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/GPContinuousOsc.cpp")
	
	
# GP osc library 
add_library (GPOsc_Lib ${SOURCE_FILES} ${INCLUDE_FILES})

target_include_directories(GPOsc_Lib PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/libs/")


set(TEST_SOURCE_FILES ${SOURCE_FILES}
					  "${CMAKE_CURRENT_SOURCE_DIR}/libs/catch/catch.hpp"
					  "${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp"
					  "${CMAKE_CURRENT_SOURCE_DIR}/test/Test_GPModel.cpp"
					  "${CMAKE_CURRENT_SOURCE_DIR}/test/Test_GPVarianceKernel.cpp"
					  "${CMAKE_CURRENT_SOURCE_DIR}/test/Test_GPRandom.cpp"
					  "${CMAKE_CURRENT_SOURCE_DIR}/test/Test_GPDrunkRNG.cpp")


add_executable(GPOsc_Test ${TEST_SOURCE_FILES} )
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/test")
add_test (NAME GPOsc_Test COMMAND GPOsc_Test)
enable_testing (true)