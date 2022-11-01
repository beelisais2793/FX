swig -lua -c++ -Iinclude/Kfr kfr.i
g++ -Iinclude/Kfr -std=c++17 -fmax-errors=1 -O2 -march=native -fPIC -shared -o kfr.so kfr_wrap.cxx -lstdc++ -lluajit -L/usr/local/lib -lkfr_dft -lkfr_io

