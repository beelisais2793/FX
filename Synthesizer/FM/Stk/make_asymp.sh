swig -Iinclude/stk -lua -c++ stk_asymp.i
gcc -fmax-errors=1 -Iinclude/stk -O2 -fPIC -march=native -mavx2 -shared -o asymp.so stk_asymp_wrap.cxx lib/libstk.a -lstdc++ -lm -lluajit
