swig -lua -c++ -I/usr/local/include/lilv-0 lilv.i
gcc -I/usr/local/include/lilv-0 -O2 -fPIC -march=native -mavx2 -shared -o lilv.so lilv_wrap.cxx -lstdc++ -lm -lluajit -llilv-0
