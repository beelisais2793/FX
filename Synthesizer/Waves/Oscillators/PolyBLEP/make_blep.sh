swig -lua -c++ -Iinclude/Oscillators TPolyBLEP.i
gcc -fmax-errors=1  -Iinclude/Oscillators -O2 -fPIC -march=native -mavx2 -shared -o TPolyBLEP.so TPolyBLEP_wrap.cxx -lstdc++ -lm -lluajit-5.1
