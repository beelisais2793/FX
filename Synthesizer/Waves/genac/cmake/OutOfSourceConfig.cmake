# Guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. "
    "Please make a new directory (called a build directory) "
    "and run cmake from there.")
endif()
