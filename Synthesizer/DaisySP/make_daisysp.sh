clear
echo 'compiling wrapper'
swig -lua -c++ -IDaisySP/Source daisysp.i
gcc -shared -fPIC -O2 -IDaisySP/Source -o daisysp.so daisysp_wrap.cxx ../lib/libDaisySP.a -lstdc++ -lluajit
