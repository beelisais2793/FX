swig -lua -c++ -IGNUPlot GNUPlot.i
gcc -O2 -IGNUPlot -fPIC -march=native -mavx2 -shared -o GNUPlot.so GNUPlot_wrap.cxx GNUPlot.cpp -lstdc++ -lm -lluajit
