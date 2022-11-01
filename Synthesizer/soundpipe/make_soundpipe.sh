swig -lua -c++ -Ih soundpipe.i
gcc -Ih -O2 -fPIC -march=native -mavx2 -shared -o soundpipe.so soundpipe_wrap.cxx ../../lib/libsoundpipe.a -lstdc++ -lm -lluajit -lfftw3 -lfftw3f -lsamplerate -lsndfile
