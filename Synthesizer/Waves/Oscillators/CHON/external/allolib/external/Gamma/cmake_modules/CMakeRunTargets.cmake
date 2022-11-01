# Basic checks
if(NOT (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})) # use only if cmake was run from the root directory
    message("Warning: The run script must be called from the source root directory." )
endif(NOT (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}))

string(REGEX MATCH ".*\\*.*" match "${CMAKE_CURRENT_SOURCE_DIR}")
IF(NOT ${match} STREQUAL "")
  message(FATAL_ERROR "Error: Please remove '*' from path!" ) # This avoids issues with the run script
ENDIF()

# Set source files
if(BUILD_DIR)
  file(GLOB GAMMA_APP_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${BUILD_APP_DIR}/*.cpp)
  string(REPLACE "/" "_" APP_NAME ${BUILD_APP_DIR})
  string(REGEX REPLACE "_+$" "" APP_NAME "${APP_NAME}")
  set(SOURCE_DIR ${BUILD_APP_DIR})
else()
  set(GAMMA_APP_SRC ${BUILD_APP_FILE})
  string(REPLACE "/" "_" APP_NAME ${BUILD_APP_FILE})
  get_filename_component(APP_NAME ${APP_NAME} NAME)
  STRING(REGEX REPLACE "\\.[^.]*\$" "" APP_NAME "${APP_NAME}")
  string(REPLACE "." "_" APP_NAME ${APP_NAME})
#  get_filename_component(APP_NAME ${APP_NAME} NAME_WE) # Get name w/o extension (extension is anything after first dot!)
  get_filename_component(SOURCE_DIR ${BUILD_APP_FILE} PATH)
endif(BUILD_DIR)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

add_executable(${APP_NAME} EXCLUDE_FROM_ALL ${GAMMA_APP_SRC})
#if(APPLE)
#  set_target_properties(${APP_NAME} PROPERTIES
#    LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
#endif(APPLE)

if(EXISTS "${SOURCE_DIR}/flags.cmake")
    include("${SOURCE_DIR}/flags.cmake")
endif()


if(EXISTS "${SOURCE_DIR}/flags.txt")
  file(READ "${SOURCE_DIR}/flags.txt" EXTRA_COMPILER_FLAGS)
  STRING(REGEX REPLACE "[\r\n]" " " EXTRA_COMPILER_FLAGS "${EXTRA_COMPILER_FLAGS}")
  set_target_properties(${APP_NAME} PROPERTIES
    COMPILE_FLAGS "${EXTRA_COMPILER_FLAGS}")
  message("WARNING: Using additional flags from ${SOURCE_DIR}/flags.txt: ${EXTRA_COMPILER_FLAGS}")
endif()

message("Target: ${APP_NAME}")
message("From sources: ${GAMMA_APP_SRC}")

get_target_property(GAMMA_LIBRARY Gamma LOCATION)
get_target_property(GAMMA_LINK_LIBRARIES Gamma GAMMA_LINK_LIBRARIES)
get_target_property(GAMMA_INCLUDE_DIR Gamma GAMMA_INCLUDE_DIR)
add_dependencies(${APP_NAME} Gamma)

# TODO copy resources to build directory

include_directories(${GAMMA_INCLUDE_DIR} )

target_link_libraries(${APP_NAME} ${GAMMA_LIBRARY} ${GAMMA_LINK_LIBRARIES})
#list(REMOVE_ITEM PROJECT_RES_FILES ${GAMMA_APP_SRC})

if(NOT RUN_IN_DEBUGGER)
add_custom_target("${APP_NAME}_run"
  COMMAND "${APP_NAME}"
  DEPENDS "${APP_NAME}"
  WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  SOURCES ${ALLOPROJECT_APP_SRC}
  COMMENT "Running: ${APP_NAME}")
  option(RUN_IN_DEBUGGER 0) # For next run
else()
add_custom_target("${APP_NAME}_run"
  COMMAND "${GAMMA_DEBUGGER}" "-ex" "run" "${EXECUTABLE_OUTPUT_PATH}/${APP_NAME}"
  DEPENDS "${APP_NAME}"
  WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  SOURCES ${ALLOPROJECT_APP_SRC}
  COMMENT "Running: ${APP_NAME}")

endif(NOT RUN_IN_DEBUGGER)
