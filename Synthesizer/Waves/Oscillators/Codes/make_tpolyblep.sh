swig -lua -c++ -Iinclude src/TPolyBLEP.i
gcc -Iinclude -fmax-errors=1 -O2 -fPIC -march=native -mavx2 -shared -o TPolyBLEP.so src/TPolyBLEP_wrap.cxx -lstdc++ -lm -lluajit
