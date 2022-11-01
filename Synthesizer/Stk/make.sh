
clear
rm gist_wrap.cxx
swig -lua -c++ -IGist/src gist.i
gcc -fmax-errors=5 -IGist/src -shared -fPIC -O2 -oluagist.so gist_wrap.cxx -lstdc++ -lluajit-5.1 -L. -lGist -lkissfft-float