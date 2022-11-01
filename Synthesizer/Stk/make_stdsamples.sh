swig -lua -c++ -Iinclude std_sample.i
gcc -fmax-errors=1 -DAUDIOFFT_FFTW3 -Iinclude -O2 -fPIC -march=native -mavx2 -shared -o std_sample.so std_sample_wrap.cxx include/AudioFFT/AudioFFT.cpp include/AudioFFT/FFTConvolver.cpp include/AudioFFT/TwoStageFFTConvolver.cpp include/AudioFFT/Utilities.cpp  -lstdc++ -lm -lluajit -lkfr_dft -lkfr_io -lfftw3 -lfftw3f
