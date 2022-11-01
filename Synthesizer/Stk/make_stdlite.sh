swig -lua -c++ stdlite.i
gcc -fpermissive -O2 -march=native -mavx2 -fPIC -shared -o stdlite.so stdlite_wrap.cxx -lstdc++ -lm -lluajit
