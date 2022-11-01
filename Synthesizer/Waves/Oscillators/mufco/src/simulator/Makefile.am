CPPFLAGS                = -I../../src/common -I../../src/graphics_engine -I../../src/simulator -I/usr/include/libxml2
lib_LTLIBRARIES         = libsimulator.la
libsimulator_la_SOURCES = ../../src/simulator/simulator.cpp \
			  ../../src/simulator/environment.cpp \
			  ../../src/simulator/oscillator.cpp
libsimulator_la_LIBADD  = -L../common -lcommon -L../graphics_engine -lGEngine -lgsl -lgslcblas -lX11 -L/usr/X11R6/lib -lpthread
