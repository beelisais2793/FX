swig -Iinclude/stk -lua -c++ stk.i
gcc -fmax-errors=1 -Iinclude/stk -O2 -fPIC -march=native -mavx2 -shared -o stk.so stk_wrap.cxx lib/libstk.a -lstdc++ -lm -lluajit -lpulse -lpulse-simple -lasound
