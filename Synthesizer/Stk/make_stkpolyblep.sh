swig -lua -c++ -Iinclude/stk stk_polyblep.i
gcc -Iinclude/stk -O2 -fPIC -march=native -mavx2 -shared -o stk_polyblep.so stk_polyblep_wrap.cxx lib/libstk.a -lstdc++ -lm -lluajit
