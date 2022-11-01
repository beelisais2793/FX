lc convolve.c
lc frames.c
lc pitch.c
lc reverse.c

convolve Spechs22.wav Synthsiz.wav TestCONV.wav
pitch Spechs22.wav


cd fft
lc fft.c
lc notchfft.c
lc peaksfft.c
lc subband.c
lc vocod.c

fft ../Spechs22.wav temp.fft
fft ../Music22.wav temp2.fft
vocod temp2.fft temp.fft temp3.fft
fft temp3.fft vocod.wav
del *.fft

cd ..
cd lpc
lc fitlpc.c
lc float2wv.c
lc lpcresyn.c
lc wv2float.c
call lpcspech.bat

cd ..

