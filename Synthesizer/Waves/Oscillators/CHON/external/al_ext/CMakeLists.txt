cmake_minimum_required(VERSION 3.10)

project("al_ext")
message("Building al_ext")

macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

list(APPEND CMAKE_MODULE_PATH
${CMAKE_CURRENT_LIST_DIR}
)

# Allolib
#add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/.." "${CMAKE_CURRENT_LIST_DIR}/build")

set(EXTENSIONS_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR})
subdirlist(SUBDIRS "${CMAKE_CURRENT_LIST_DIR}")

#include_directories("${CMAKE_CURRENT_LIST_DIR}/../include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/..")

add_custom_target(al_ext)
message("al_ext at: ${CMAKE_CURRENT_LIST_DIR}")
foreach(subdir ${SUBDIRS})
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${subdir}/CMakeLists.txt")
    add_subdirectory(${subdir})
    if (TARGET al_${subdir})
      message(" * Extension: ${subdir}")
      add_dependencies(al_ext al_${subdir})
      list(APPEND AL_EXT_LIBRARIES al_${subdir})
    else()
      message(" - NOT BUILDING Extension: ${subdir}")
    endif()
  endif()
endforeach()
message("** DONE Processing extensions **")

# Expose libraries to the outside
set_target_properties(al_ext PROPERTIES AL_EXT_LIBRARIES "${AL_EXT_LIBRARIES}")
