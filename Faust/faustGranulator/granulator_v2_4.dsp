// USER INTERFACE //

volumeSlider = hslider("Volume", 0.5, 0, 1, 0.01); // volume slider
grainbufferSizeSlider = hslider("Grain Size", 0.1, 0.01, 0.5, 0.01); // grain size in samples
delayLengthSlider = hslider("Delay Length", 2, 0.5, 2, 0.1); // delay length in samples
pitchSlider = hslider("Pitch", 1, 0.1, 2, 0.01); // pitch control / downsampling

// CODE //

SR = 44100; // samplerate in samples per second
N = 20; // numbers of grains
Volume = volumeSlider;

bufferSize = 2 * SR; // size of input buffer in samples
bufferCounter = + (1) % delayLength ~ _; // counter to cycle through the input buffer from 0 to bufferSize
delayLength = SR * delayLengthSlider; // set delay length with delay length slider

grainbufferSize = SR * grainbufferSizeSlider; // size of grainbuffer in samples
grainbufferCounter = + (pitchSlider) % grainbufferSize ~ _; // counter to cycle through the grains from 0 to grainSize

SH(trigger, signal) = ( * (1 - trigger) + signal * trigger) ~ _; // sample and hold function definiton for grain offset
grainOffset(i) = int(SH(1 - 1', int(delayLength * noise(i)))); // delay length between grainbuffer refresh
grainCounter(i) = (grainbufferCounter + grainOffset(i)) % grainbufferSize; // grain-specific grain counter
grainRandomStartPos(i) = int(SH(int(grainCounter(i) / (grainbufferSize - 1)), int(delayLength * noise(i)))); // individual random startingposition for each grain
grainPosition(i) = grainCounter(i) + grainRandomStartPos(i); // 

buffer(writeIndex, readIndex, signal) = rwtable(bufferSize, 0.0, int(writeIndex % delayLength), signal, int(readIndex % delayLength)); // function definition of cycling buffer

window(i) = sin(2 * 3.14159 * grainCounter(i) / (grainbufferSize - 1)); // window function

// NOISE GENERATOR //

RANDMAX = (2147483647.0) * 2 ; // max number of random number generatorn

polyRandom(n) = scramble(n) ~ _ 
    with {
        scramble (1) = * (1103515245) : + (12345); // pseudo random number generator
        scramble (n) = scramble(1) <: scramble(n - 1), _;
};

polyNoise(n) = polyRandom(n) : par(i, n, /(RANDMAX));

noiseChan(n, 0) = polyNoise(N + 1) :> _, par(j, n - 1, !);
noiseChan(n, i) = polyNoise(N + 1) :> par(j, i, !), _, par(j, n - i - 1, !);

noise(i) = (noiseChan(N + 1, i) + 1) / 2; //to get nth channel of multi-channel noiser (not quite sure how)

// PROCESS //

process = _ <: par(i, N, buffer(int(bufferCounter), int(grainPosition(i)), _) * window(i) * Volume);// * (i < N) / N);// :> _, _;