swig -lua -c++ sample.i
gcc -fmax-errors=1 -O2 -fPIC -march=native -mavx2 -shared -o sample.so sample_wrap.cxx -lstdc++ -lm -lluajit
