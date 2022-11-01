swig -lua -c++ -Iinclude/Oscillators TWaveTable.i
gcc -fmax-errors=1  -Iinclude/Oscillators -O2 -fPIC -march=native -mavx2 -shared -o TWaveTable.so TWaveTable_wrap.cxx -lstdc++ -lm -lluajit-5.1
