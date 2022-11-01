SUBDIRS                   = common graphics_engine simulator
CPPFLAGS                  = -I../src/common -I../src/graphics_engine -I../src/simulator
bin_PROGRAMS              = mufcoSim
mufcoSim_SOURCES          = ../src/main.cpp
mufcoSim_LDADD            = -Lsimulator -lsimulator
mufcoSim_LDFLAGS          = -static
