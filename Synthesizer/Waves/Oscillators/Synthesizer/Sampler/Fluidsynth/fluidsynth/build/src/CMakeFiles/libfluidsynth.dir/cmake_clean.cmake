file(REMOVE_RECURSE
  "libfluidsynth.pdb"
  "libfluidsynth.so"
  "libfluidsynth.so.3"
  "libfluidsynth.so.3.0.5"
)

# Per-language clean rules from dependency scanning.
foreach(lang C CXX)
  include(CMakeFiles/libfluidsynth.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
