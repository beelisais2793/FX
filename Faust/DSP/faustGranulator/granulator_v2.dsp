// USER INTERFACE //
grainOffsetSlider = hslider("Grain Offset", 100, 100, 10000, 1); // grain offset in samples
grainSizeSlider = hslider("Grain Size", 1000, 1000, 10000, 1); // grain size in samples

// CODE //
SR = 44100; // samplerate in samples per second
N = 10; // numbers of grains

bufferSize = SR; // size of input buffer in samples
bufferCounter = + (1) % bufferSize ~ _; // counter to cycle through the input buffer from 0 to bufferSize

grainSize = grainSizeSlider; // size of grains in samples
grainCounter = + (1) % grainSize ~ _; // counter to cycle through the grains from 0 to grainSize
grainOffset = grainOffsetSlider; // offset between grains in samples

buffer(writeIndex, readIndex, signal) = rwtable(bufferSize, 0.0, writeIndex, signal, readIndex); // function definition of cycling buffer

// PROCESS //
process = _ <: par(i, N, buffer(int(bufferCounter), int(grainCounter + (i * grainOffset)), _)) :> _, _; // N cycling buffers in parallel; bufferCounter as writeIndex,
                                                                                                        // grainCounter + (i * grainOffset) as readIndex with i as index of one single
                                                                                                        // cycling buffer
