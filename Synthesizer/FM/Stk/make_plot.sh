swig -lua -c++ gnuplot.i
gcc -fpermissive -O2 -fPIC -shared -o plot.so gnuplot_wrap.cxx gnuplot_i.c -lstdc++ -lm -lluajit
