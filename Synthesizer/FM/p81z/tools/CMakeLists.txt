find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(JACK "jack")
endif()

add_executable(read_syx_patches
  "read_syx_patches.cpp")
target_link_libraries(read_syx_patches
  PRIVATE P81Z)

if(JACK_FOUND)
  add_executable(simple_synth
    "simple_synth.cpp"
    "simple_synth.h")
  target_link_libraries(simple_synth
    PRIVATE P81Z)
  target_include_directories(simple_synth
    PRIVATE ${JACK_INCLUDE_DIRS})
  target_link_libraries(simple_synth
    PRIVATE ${JACK_LIBRARIES})
endif()
