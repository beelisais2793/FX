cmake_minimum_required(VERSION 3.0)

project(additivesynth)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Ofast")

add_executable(additivesynth 
	main.cpp)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(additivesynth SDL2)

#install(TARGETS additivesynth RUNTIME DESTINATION bin)
