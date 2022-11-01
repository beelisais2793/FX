swig -lua -c++ DSPLibrary.i
gcc -O2 -fPIC -march=native -mavx2 -shared -o DSPLibrary.so DSPLibrary_wrap.cxx libDSPLibrary.a -lstdc++ -lm -lluajit
