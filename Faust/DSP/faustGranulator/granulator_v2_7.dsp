declare name "Multichannel Data Granulator"; 
declare version "2.7";
declare author "Lukas Hartmann & Luca Hilbrich"; 

declare options "[osc:on]";
declare options "[midi:on]";
import("stdfaust.lib");

// USER INTERFACE //

volumeSlider = vslider("h:/h:[0]Granulator/ [0]Volume [midi:ctrl 0]", 0.5, 0, 1, 0.01); // volume slider
pitchSlider = vslider("h:/h:[0]Granulator/ [1] Pitch [midi:ctrl 1]", 1, -2, 2, 0.01); // pitch control (downsampling)
windowSlider = vslider("h:/h:[0]Granulator/ [2]Window[style:radio{'Sine Window':window1;'Hamming Window':window2;'Flat Top Window':window3}] [midi:ctrl 2]",1,1,3,1);

grainAmountSlider =  vslider("h:/h:[1]Grain/ [1] Density [midi:ctrl 3]", 1, 1, MAXN, 1); // number of grains
grainbufferSizeSlider = vslider("h:/h:[1]Grain/ [2] Size [midi:ctrl 4]", 0.1, 0.01, 0.5, 0.01); // grain size in samples
delayLengthSlider = vslider("h:/h:[1]Grain/ [3] Delay [midi:ctrl 5]", 2, 0.5, 2, 0.1); // delay length in samples
positionspreadSlider = vslider("h:/h:[1]Grain/ [4] Spread [midi:ctrl 6]", 1, 0.1, 2, 0.1); // position spread of grains

reverb_spread_slider = vslider("h:/v:[2]Effects/ [0]Reverb Spread[unit:%][style:knob][midi:ctrl 16]", 0, 0, 100, 1);
dry_wet = vslider("h:/v:[2]Effects/ [1]Dry Wet[style:knob] [midi:ctrl 17]", 0.4, 0, 1, 0.001);
filter_cuttoff = vslider("h:/v:[2]Effects/ [2]HPF[unit:Hz][style:knob] [midi:ctrl 18]", 100, 100, 10000, 1);



// CODE //

SR = 44100; // samplerate in samples per second
MAXN = 10; // maximum numbers of grains
Volume = volumeSlider; // volume slider

bufferSize = 2 * SR; // size of input buffer in samples
bufferCounter = + (1) % delayLength ~ _; // counter to cycle through the input buffer from 0 to bufferSize
delayLength = SR * delayLengthSlider; // set delay length with delay length slider

grainbufferSize = SR * grainbufferSizeSlider; // size of grainbuffer in samples
grainbufferCounter = + (pitchSlider) % grainbufferSize ~ _; // counter to cycle through the grains from 0 to grainSize

SH(trigger, signal) = ( * (1 - trigger) + signal * trigger) ~ _; // sample and hold function definiton for grain offset
grainOffset(i) = int(SH(1 - 1', int(delayLength * noise(i)))); // delay length between grainbuffer refresh
grainCounter(i) = (grainbufferCounter + grainOffset(i)) % grainbufferSize; // grain-specific grain counter
grainRandomStartPos(i) = int(SH(int(grainCounter(i) / (grainbufferSize - 1)), int(delayLength * noise(i)))); 
grainPosition(i) = grainCounter(i) + grainRandomStartPos(i);


buffer(writeIndex, readIndex, signal) = rwtable(bufferSize, 0.0, int(writeIndex % delayLength), signal, int(readIndex % delayLength)); // function definition of cycling buffer

window1(i) = sin(2 * 3.14159 * grainCounter(i) / (grainbufferSize - 1)); // window function
window2(i) = 0.54 - 0.64 * cos(2 * 3.14159 * grainCounter(i) / grainbufferSize - 1); // hamming window
window3(i) = 1 - 1.93 * cos(2 * 3.14159 * grainCounter(i) / grainbufferSize - 1) + 1.29 * cos(4 * 3.14159 * grainCounter(i) / grainbufferSize - 1) - 0.388 * cos(6 * 3.14159 * grainCounter(i) / grainbufferSize - 1) + 0.028 * cos(8 * 3.14159 * grainCounter(i) / grainbufferSize - 1); // flat top window

// NOISE GENERATOR //

RANDMAX = (2147483647.0) * 2 ; // max number of random number generatorn

polyRandom(n) = scramble(n) ~ _ 
    with {
        scramble (1) = * (1103515245) : + (12345); // pseudo random number generator
        scramble (n) = scramble(1) <: scramble(n - 1), _;
};

polyNoise(n) = polyRandom(n) : par(i, n, /(RANDMAX));

noiseChan(n, 0) = polyNoise(MAXN + 1) :> _, par(j, n - 1, !);
noiseChan(n, i) = polyNoise(MAXN + 1) :> par(j, i, !), _, par(j, n - i - 1, !);

noise(i) = (noiseChan(MAXN + 1, i) + 1) / 2 * positionspreadSlider; //to get nth channel of multi-channel noiser (not quite sure how)

// EFFECTS //

reverb = _, _ <: ( * (dry_wet) * fixedgain, * (dry_wet) * fixedgain : 
    re.stereo_freeverb(0.5, 0.5, 0.5, reverb_spread_slider)),
    * (1 - dry_wet), * (1 - dry_wet) :> _, _
    with {
        fixedgain = 0.1;
        
    }; // stereo reverb with dry/wet implementation

hpf_filter =  _, _: (fi.highpass(filter_order, filter_cuttoff)), (fi.highpass(filter_order, filter_cuttoff)): _, _
    with {
        filter_order = 1;
    }; //stereo hpf 

// PROCESS //

process = _ <: par(i, MAXN, buffer(int(bufferCounter), int(grainPosition(i)), _) * windowSlider * Volume * (i < grainAmountSlider) / grainAmountSlider) :> reverb :> hpf_filter :> _, _;