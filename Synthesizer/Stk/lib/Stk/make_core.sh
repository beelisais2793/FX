swig -lua -c++ -I/usr/local/include -Iinclude/Kfr src/kfr_core.i
gcc -std=c++17 -fmax-errors=1 -Iinclude/Kfr -I/usr/local/include/python3.9 -Ikfr/include -O2 -fPIC -march=native -mavx2 -shared -o kfr_core.so src/kfr_core_wrap.cxx -lstdc++ -lm -lluajit -lkfr_dft -lkfr_io 
