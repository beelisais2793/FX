CPPFLAGS                  = -I../../src/simulator -I../../src/common -I../../src/graphics_engine -I/usr/X11R6/include -I/usr/include/gsl
bin_PROGRAMS              = mufcoSim_parallel
mufcoSim_parallel_SOURCES = ../../src/parallel/main.cpp ../../src/common/tinyxml2.cpp
mufcoSim_parallel_LDADD   = -L../simulator -lsimulator
mufcoSim_parallel_LDFLAGS = -static
