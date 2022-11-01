declare filename "stream-splice"; 
declare version "1.0";
declare author "Lukas Hartmann & Luca Hilbrich"; 

declare options "[osc:on]";
declare options "[midi:on]";
import("stdfaust.lib");

// USER INTERFACE //


volumeSlider = vslider("h:/h:[0]Main/ [0]Input Gain [midi:ctrl 0]", 0.5, 0, 1, 0.01); // volume slider
dry_wetMain = vslider("h:/h:[0]Main/ [1]Dry/Wet [midi:ctrl 1]", 1.0, 0, 1, 0.001);
pitchSliderA = vslider("h:/h:[0]Main/ [2] Pitch Alpha [midi:ctrl 2]", 0, -2, 2, 0.01); // pitch control (downsampling)
pitchSliderB = vslider("h:/h:[0]Main/ [2] Pitch Beta [midi:ctrl 3]", 0, -2, 2, 0.01); // pitch control (downsampling)

grainAmountSlider =  vslider("h:/h:[1]Granulator/v:Grain/h:[0]Control/[0]Density [midi:ctrl 4]",5, 1, MAXN, 1); // number of grains
grainbufferSizeSlider = vslider("h:/h:[1]Granulator/v:Grain/h:[0]Control/[1]Size [midi:ctrl 5]", 0.1, 0.01, 0.5, 0.01); // grain size in samples
delayLengthSlider = vslider("h:/h:[1]Granulator/v:Grain/h:[0]Control/[2]Delay [midi:ctrl 6]", 1, 0.5, 2, 0.1); // delay length in samples
positionspreadSlider = vslider("h:/h:[1]Granulator/v:Grain/h:[0]Control/[3]Spread [midi:ctrl 7]", 1, 0.1, 2, 0.1); // position spread of grains
windowSliderA = vslider("h:/h:[1]Granulator/v:Grain/h:[1]Window/[1] Alpha [style:menu{'Sine':0;
'Hanning ':1;'Top Down':2}]",0,0,2,1); // alpha window selector
windowSliderB = vslider("h:/h:[1]Granulator/v:Grain/h:[1]Window/[1] Beta [style:menu{'Sine':0;'Hanning ':1;'Top Down':2}]",0,0,2,1); // beta window selector


dry_wetA = vslider("h:/h:[2]Mixer/v:[2]Alpha/ [0]Reverb[style:knob] [midi:ctrl 16]", 0.0, 0, 1, 0.001);
filter_cuttoffA = vslider("h:/h:[2]Mixer/v:[2]Alpha/ [1]HPF[unit:Hz][style:knob] [midi:ctrl 17]", 100, 100, 10000, 1);
volumeSliderAL = vslider("h:/h:[2]Mixer/v:[2]Alpha/ [2]GainL[style:knob] [midi:ctrl 18]", 1.0, 0, 1, 0.01);
volumeSliderAR = vslider("h:/h:[2]Mixer/v:[2]Alpha/ [3]GainR[style:knob] [midi:ctrl 19]", 1.0, 0, 1, 0.01);

dry_wetB = vslider("h:/h:[2]Mixer/v:[3]Beta/ [0]Reverb[style:knob] [midi:ctrl 20]", 0.0, 0, 1, 0.001);
filter_cuttoffB = vslider("h:/h:[2]Mixer/v:[3]Beta/ [1]HPF[unit:Hz][style:knob] [midi:ctrl 21]", 100, 100, 10000, 1);
volumeSliderBL = vslider("h:/h:[2]Mixer/v:[3]Beta/ [2]GainL[style:knob] [midi:ctrl 22]", 1.0, 0, 1, 0.01);
volumeSliderBR = vslider("h:/h:[2]Mixer/v:[3]Beta/ [3]GainR[style:knob] [midi:ctrl 23]", 1.0, 0, 1, 0.01);

// CODE //

SR = 44100; // samplerate in samples per second
MAXN = 10; // maximum numbers of grains
Volume = volumeSlider; // volume slider

bufferSize = 2 * SR; // size of input buffer in samples
bufferCounter = + (1) % delayLength ~ _; // counter to cycle through the input buffer from 0 to bufferSize
delayLength = SR * delayLengthSlider; // set delay length with delay length slider

grainbufferSize = SR * grainbufferSizeSlider; // size of grainbuffer in samples

grainbufferCounterA = + (pitchSliderA) % grainbufferSize ~ _; // counter to cycle through the grains from 0 to grainSize
grainbufferCounterB = + (pitchSliderB) % grainbufferSize ~ _; // counter to cycle through the grains from 0 to grainSize

SH(trigger, signal) = ( * (1 - trigger) + signal * trigger) ~ _; // sample and hold function definiton for grain offset
grainOffset(i) = int(SH(1 - 1', int(delayLength * noise(i)))); // delay length between grainbuffer refresh

grainCounterA(i) = (grainbufferCounterA + grainOffset(i)) % grainbufferSize; // grain-specific grain counter (Alpha)
grainCounterB(i) = (grainbufferCounterB + grainOffset(i)) % grainbufferSize; // grain-specific grain counter (Beta)

grainRandomStartPosA(i) = int(SH(int(grainCounterA(i) / (grainbufferSize - 1)), int(delayLength * noise(i)))); 
grainRandomStartPosB(i) = int(SH(int(grainCounterB(i) / (grainbufferSize - 1)), int(delayLength * noise(i)))); 

grainPositionA(i) = grainCounterA(i) + grainRandomStartPosA(i);
grainPositionB(i) = grainCounterB(i) + grainRandomStartPosB(i);

buffer(writeIndex, readIndex, signal) = rwtable(bufferSize, 0.0, int(writeIndex % delayLength), signal, int(readIndex % delayLength)); // function definition of cycling buffer

// alpha windows
window1A(i) = sin(2 * 3.14159 * grainCounterA(i) / (grainbufferSize - 1)); // sine
window2A(i) = 0.54 - 0.64 * cos(2 * 3.14159 * grainCounterA(i) / grainbufferSize - 1); // hamming
window3A(i) = 1 - 1.93 * cos(2 * 3.14159 * grainCounterA(i) / grainbufferSize - 1) + 1.29 * cos(4 * 3.14159 * grainCounterA(i) / grainbufferSize - 1) - 0.388 * cos(6 * 3.14159 * grainCounterA(i) / grainbufferSize - 1) + 0.028 * cos(8 * 3.14159 * grainCounterA(i) / grainbufferSize - 1); // flat top

// beta windows
window1B(i) = sin(2 * 3.14159 * grainCounterB(i) / (grainbufferSize - 1)); //sine
window2B(i) = 0.54 - 0.64 * cos(2 * 3.14159 * grainCounterB(i) / grainbufferSize - 1); // hamming
window3B(i) = 1 - 1.93 * cos(2 * 3.14159 * grainCounterB(i) / grainbufferSize - 1) + 1.29 * cos(4 * 3.14159 * grainCounterB(i) / grainbufferSize - 1) - 0.388 * cos(6 * 3.14159 * grainCounterB(i) / grainbufferSize - 1) + 0.028 * cos(8 * 3.14159 * grainCounterB(i) / grainbufferSize - 1); // flat top

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

reverbA = ( * (dry_wetA) * fixedgain:re.mono_freeverb(fb1, fb2, damp, spread)), * (1 - dry_wetA)
    with {
        fb1 = 0.5; // coefficient of the lowpass comb filters (0-1)
        fb2 = 0.5; // coefficient of the allpass comb filters (0-1)
        damp = 0.5; // damping of the lowpass comb filter (0-1)
        spread = 1; //spatial spread in number of samples (for stereo)
        fixedgain = 0.1;
    }; // mono with dry/wet implementation

hpf_filterA =  (fi.highpass(filter_order, filter_cuttoffA)) 
    with {
        filter_order = 1;
    }; 

reverbB = ( * (dry_wetB) * fixedgain:re.mono_freeverb(fb1, fb2, damp, spread)), * (1 - dry_wetB)
    with {
        fb1 = 0.5; // coefficient of the lowpass comb filters (0-1)
        fb2 = 0.5; // coefficient of the allpass comb filters (0-1)
        damp = 0.5; // damping of the lowpass comb filter (0-1)
        spread = 1; //spatial spread in number of samples (for stereo)
        fixedgain = 0.1;
    }; // mono with dry/wet implementation

hpf_filterB =  (fi.highpass(filter_order, filter_cuttoffB)) 
    with {
        filter_order = 1;
    }; 

// GRANULATORS //

granAlpha = par(i, MAXN, buffer(int(bufferCounter), int(grainPositionA(i)), _) * (select3(windowSliderA,window1A(i),window2A(i),window3A(i))) * 
(i < grainAmountSlider) / grainAmountSlider);

granBeta = par(i, MAXN, buffer(int(bufferCounter), int(grainPositionB(i)), _) * (select3(windowSliderB,window1B(i),window2B(i),window3B(i)))* 
(i < grainAmountSlider) / grainAmountSlider);

// PROCESS //

process =

_ * Volume <: 

((((granAlpha:>_*dry_wetMain<:reverbA:> hpf_filterA * volumeSliderAL),(granBeta:>_*dry_wetMain<:reverbB:> hpf_filterB * volumeSliderBL):>_),_ * (1 - dry_wetMain)):>_)
,
((((granAlpha:>_*dry_wetMain<:reverbA:> hpf_filterA * volumeSliderAR),(granBeta:>_*dry_wetMain<:reverbB:> hpf_filterB * volumeSliderBR):>_),_* (1 - dry_wetMain)):>_)

;