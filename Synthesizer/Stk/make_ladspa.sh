swig -lua -c++ -Iinclude/ladspamm-1 ladspa.i
gcc -Iinclude -Iinclude/ladspamm-1 -O2 -fPIC -shared -march=native -mavx2 -o ladspa.so ladspa_wrap.cxx -lstdc++ -lm -lluajit -lboost_filesystem
