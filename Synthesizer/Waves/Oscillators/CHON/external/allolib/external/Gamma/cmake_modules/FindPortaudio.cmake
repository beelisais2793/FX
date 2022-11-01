# - Try to find PORTAUDIO
# Once done this will define
#
#  PORTAUDIO_FOUND - system has PORTAUDIO
#  PORTAUDIO_INCLUDE_DIR - the PORTAUDIO include directory
#  PORTAUDIO_LIBRARIES - Link these to use PORTAUDIO
#

include(LibFindMacros)

# Dependencies
#libfind_package(LO Magick)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(PORTAUDIO_PKGCONF portaudio)

# Include dir
find_path(PORTAUDIO_INCLUDE_DIR
  NAMES portaudio.h
  PATHS ${PORTAUDIO_PKGCONF_INCLUDE_DIRS}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      "${CMAKE_CURRENT_SOURCE_DIR}/external/include"
)

# Finally the library itself
find_library(PORTAUDIO_LIBRARY
  NAMES portaudio portaudio_x86
  PATHS ${PORTAUDIO_PKGCONF_LIBRARY_DIRS}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      "${CMAKE_CURRENT_SOURCE_DIR}/external/lib_win32"
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(PORTAUDIO_PROCESS_INCLUDES PORTAUDIO_INCLUDE_DIR)
set(PORTAUDIO_PROCESS_LIBS PORTAUDIO_LIBRARY)
libfind_process(PORTAUDIO)
