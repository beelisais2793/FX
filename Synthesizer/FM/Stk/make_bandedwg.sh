swig -Iinclude/stk -lua -c++ stk_bandedwg.i
gcc -fmax-errors=1 -Iinclude/stk -O2 -fPIC -march=native -mavx2 -shared -o bandedwg.so stk_bandedwg_wrap.cxx lib/libstk.a -lstdc++ -lm -lluajit
