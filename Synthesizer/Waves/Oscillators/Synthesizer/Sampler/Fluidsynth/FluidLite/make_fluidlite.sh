swig -lua -c++ -Iinclude -Iinclude/fluidsynth fluidlite.i
gcc -I. -Iinclude -O2 -fPIC -march=native -mavx2 -shared -o fluidlite.so fluidlite_wrap.cxx build/libfluidlite.a -lstdc++ -lm -lluajit
