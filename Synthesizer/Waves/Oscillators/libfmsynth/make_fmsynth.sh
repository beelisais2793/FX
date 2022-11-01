swig -lua -c++ -Ilibfmsynth/include fmsynth.i
gcc -fmax-errors=1 -Ilibfmsynth/include -O2 -fPIC -march=native -mavx2 -shared -o fmsynth.so fmsynth_wrap.cxx libfmsynth.a -lstdc++ -lm -lluajit
