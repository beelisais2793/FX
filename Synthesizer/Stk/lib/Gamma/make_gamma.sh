swig -lua -c++ -IGamma gamma.i
gcc -fmax-errors=1 -IGamma -O2 -fPIC -march=native -mavx2 -shared -o gamma.so gamma_wrap.cxx libGamma.a -lstdc++ -lm -lportaudio -lluajit
