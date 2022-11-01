cmake_minimum_required(VERSION 2.8.9)
project (nebula)

SET(CMAKE_CXX_FLAGS "-Wall -g -O2 -std=c++17 -I . -Wno-return-type-c-linkage")

add_executable(nebula nebula.cpp Envelope.h AbstractOscillator.h GetOpt.h Granulator.h OscillatorF.h OscillatorI.h Phasor.h WavFile.h)

INSTALL(
    TARGETS nebula 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION $ENV{HOME}/.nebula
    ARCHIVE DESTINATION $ENV{HOME}/.nebula)
