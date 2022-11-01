swig -lua -c++ -I/usr/local/include/lilv-0/lilv lilv.i
gcc -I/usr/local/incluce/lilv-0 -I/usr/local/include/lilv-0/lilv -O2 -fPIC -march=native -mavx2 -shared -o lilv.so lilv_wrap.cxx -lstdc++ -lm -lluajit -llilv-0
