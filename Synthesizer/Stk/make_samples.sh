swig -lua -c++ samples.i
gcc -O2 -fPIC -march=native -mavx2 -shared -o samples.so samples_wrap.cxx -lstdc++ -lm -lluajit
