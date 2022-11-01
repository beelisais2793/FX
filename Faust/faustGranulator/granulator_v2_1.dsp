// USER INTERFACE //
grainOffsetSlider = hslider("Grain Offset", 100, 100, 10000, 1); // grain offset in samples
grainSizeSlider = hslider("Grain Size", 1000, 1000, 100000, 1); // grain size in samples

// CODE //
SR = 44100; // samplerate in samples per second
N = 1; // numbers of grains

bufferSize = 3*SR; // size of input buffer in samples
bufferCounter = + (1) % bufferSize ~ _; // counter to cycle through the input buffer from 0 to bufferSize

grainSize = grainSizeSlider; // size of grains in samples
grainCounter = + (1) % grainSize ~ _; // counter to cycle through the grains from 0 to grainSize
grainOffset(i) = grainOffsetSlider * i; // offset from first grain in samples
grainPosition(i) = (grainCounter + grainOffset(i));

buffer(writeIndex, readIndex, signal) = rwtable(bufferSize, 0.0, writeIndex, signal, readIndex); // function definition of cycling buffer

window(i) = sin(2 * 3.14159 * grainPosition(i) / (grainSize - 1));
// window(i) = 0.5 - 0.46 * cos((2 * 3.141592 * grainPosition(i)) / grainSize - 1); // hamming window (clipping?)

// PROCESS //
process = _ <: par(i, N, buffer(int(bufferCounter), int(grainCounter), _) * window(i)) :> _ <: _, _; // N cycling buffers in parallel; bufferCounter as writeIndex, grainCounter + (i                                                                                                       // * grainOffset) as readIndex with i as index of one single cycling buffer