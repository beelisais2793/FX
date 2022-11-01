swig -lua -c++ fftconvolver.i
gcc -O2 -fPIC -march=native -mavx2 -shared -o fftconvolver.so fftconvolver_wrap.cxx FFTConvolver.cpp TwoStageFFTConvolver.cpp AudioFFT.cpp Utilities.cpp -lstdc++ -lm -lluajit-5.1
