swig -lua -c++ std_math.i
gcc -fmax-errors=1  -O2 -fPIC -march=native -mavx2 -shared -o stdmath.so std_math_wrap.cxx -lstdc++ -lm -lluajit
