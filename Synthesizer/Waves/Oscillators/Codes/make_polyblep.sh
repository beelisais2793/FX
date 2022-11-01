swig -lua -c++ -Iinclude src/PolyBLEP.i
gcc -Iinclude -O2 -fPIC -march=native -mavx2 -shared -o PolyBLEP.so src/PolyBLEP_wrap.cxx src/PolyBLEP.cpp -lstdc++ -lm -lluajit
