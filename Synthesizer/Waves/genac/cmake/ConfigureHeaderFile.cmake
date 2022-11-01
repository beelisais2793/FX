# Put config header into build dir of the project.
macro(ConfigureHeaderFile _header)
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/${_header}.in"
  "${CMAKE_BINARY_DIR}/${_header}"
  @ONLY
  )
include_directories("${CMAKE_BINARY_DIR}")
endmacro()
