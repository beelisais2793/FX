swig -lua -c++ pink.i
gcc -O2 -fPIC -march=native -mavx2 -shared -o pink.so pink_wrap.cxx libpink.a -lstdc++ -lm -lluajit
