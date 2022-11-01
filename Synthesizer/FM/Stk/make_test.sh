gcc -std=c++17 -D__LINUX_PULSE__ -fmax-errors=1 -o test test.cpp Stk.cpp PolyBLEP.cpp lib/libaudiofft.a -lstdc++ -lm -lkfr_dft -lkfr_io -lsndfile -lpthread
