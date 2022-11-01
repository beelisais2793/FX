cmake_minimum_required(VERSION "3.5")

project(P81Z)

add_library(P81Z STATIC
  "sources/FMArrangement.cpp"
  "sources/FMArrangement.h"
  "sources/FMOperator.cpp"
  "sources/FMOperator.h"
  "sources/FMPatch.cpp"
  "sources/FMPatch.h"
  "sources/FMSynth.cpp"
  "sources/FMSynth.h"
  "sources/MIDI.h"
  "sources/TX81Z/TX81Z_common.cpp"
  "sources/TX81Z/TX81Z_common.h"
  "sources/TX81Z/TX81Z_envelope.cpp"
  "sources/TX81Z/TX81Z_envelope.h"
  "sources/TX81Z/TX81Z_extra.cpp"
  "sources/TX81Z/TX81Z_extra.h"
  "sources/TX81Z/TX81Z_oscillator.cpp"
  "sources/TX81Z/TX81Z_oscillator.h"
  "sources/TX81Z/TX81Z_output_filter.cpp"
  "sources/TX81Z/TX81Z_output_filter.h")
target_include_directories(P81Z
  PUBLIC "sources"
  PUBLIC "thirdparty/pl_list")

add_subdirectory(tools)
