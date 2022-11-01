swig -Iinclude/stk -lua -c++ stk_adsr.i
gcc -fmax-errors=1 -Iinclude/stk -O2 -fPIC -march=native -mavx2 -shared -o adsr.so stk_adsr_wrap.cxx lib/libstk.a -lstdc++ -lm -lluajit
