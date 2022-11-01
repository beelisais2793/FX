swig -lua -c++ ladspa.i
gcc -O2 -fPIC -shared -march=native -mavx2 -o ladspa.so ladspa_wrap.cxx -lstdc++ -lm -lluajit -lboost_filesystem
