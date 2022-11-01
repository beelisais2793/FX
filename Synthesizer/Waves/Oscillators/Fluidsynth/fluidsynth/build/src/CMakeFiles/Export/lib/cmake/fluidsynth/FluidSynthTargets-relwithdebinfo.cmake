#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FluidSynth::fluidsynth" for configuration "RelWithDebInfo"
set_property(TARGET FluidSynth::fluidsynth APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(FluidSynth::fluidsynth PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/fluidsynth"
  )

list(APPEND _IMPORT_CHECK_TARGETS FluidSynth::fluidsynth )
list(APPEND _IMPORT_CHECK_FILES_FOR_FluidSynth::fluidsynth "${_IMPORT_PREFIX}/bin/fluidsynth" )

# Import target "FluidSynth::libfluidsynth" for configuration "RelWithDebInfo"
set_property(TARGET FluidSynth::libfluidsynth APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(FluidSynth::libfluidsynth PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libfluidsynth.so.3.0.5"
  IMPORTED_SONAME_RELWITHDEBINFO "libfluidsynth.so.3"
  )

list(APPEND _IMPORT_CHECK_TARGETS FluidSynth::libfluidsynth )
list(APPEND _IMPORT_CHECK_FILES_FOR_FluidSynth::libfluidsynth "${_IMPORT_PREFIX}/lib/libfluidsynth.so.3.0.5" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
