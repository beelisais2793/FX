// USER INTERFACE //
volumeSlider = hslider("Volume", 0.5, 0, 1, 0.01); // volume slider
grainbufferSizeSlider = hslider("Grain Size", 1000, 1000, 10000, 1); // grain size in samples
delayLengthSlider = hslider("Delay Length", 1000, 1000, 10000, 1); // delay length in samples
pitchSlider = hslider("Pitch", 1, 0.1, 2, 0.1); // pitch control/ downsampling

// CODE //
SR = 44100; // samplerate in samples per second
N = 1; // numbers of grains
Volume = volumeSlider;

bufferSize = SR; // size of input buffer in samples
bufferCounter = + (1) % delayLength ~ _; // counter to cycle through the input buffer from 0 to bufferSize
delayLength = delayLengthSlider; // set delay length with delay length slider

grainbufferSize = grainbufferSizeSlider; // size of grainbuffer in samples
grainbufferCounter = + (pitchSlider) % grainbufferSize ~ _; // counter to cycle through the grains from 0 to grainSize

SH(trigger, signal) = ( * (1 - trigger) + signal * trigger) ~ _; // sample and hold function definiton for grain offset
grainOffset(i) = int(SH(1 - 1', int(delayLength))); // delay length between grainbuffer refresh
grainCounter(i) = (grainbufferCounter + grainOffset(i)) % grainbufferSize; // grain-specific grain counter
// grainPosition(i) = grainCounter(i) + random(i);

buffer(writeIndex, readIndex, signal) = rwtable(bufferSize, 0.0, int(writeIndex % delayLength), signal, int(readIndex % delayLength)); // function definition of cycling buffer

window(i) = sin(2 * 3.14159 * grainCounter(i) / (grainbufferSize - 1)); // window function

// PROCESS //
process = _ <: par(i, N, buffer(int(bufferCounter), int(grainCounter(i)), _) * window(i) * Volume * (i < N) / N) :> _ <: _, _;
