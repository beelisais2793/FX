# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/quake/Projects/fluidsynth/fluidsynth/src/gentables"
  "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables"
  "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables-prefix"
  "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables-prefix/tmp"
  "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables-prefix/src/gentables-stamp"
  "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables-prefix/src"
  "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables-prefix/src/gentables-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/quake/Projects/fluidsynth/fluidsynth/build/src/gentables-prefix/src/gentables-stamp/${subDir}")
endforeach()
