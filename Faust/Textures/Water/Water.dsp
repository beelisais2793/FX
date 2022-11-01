import("stdfaust.lib");
//Utilities-----------
lfNoise(freq) = no.lfnoise0(freq):si.smooth(ba.tau2pole(1/freq));
randRangeNoInt(freq, lo, hi) = no.lfnoise0(freq)*((hi-lo)*0.5)+(lo+((hi-lo)*0.5));
dust(freq, thresh) = no.lfnoise(freq) >(thresh);
clip(in, thresh) = in : min(thresh) : max(thresh * -1);

waterflow = hslider("waterflow", 140, 100, 400,0.01);
trigs = dust(waterflow, 0.7);
env = en.ar(0.015,1,trigs);
freq = ba.midikey2hz(randRangeNoInt(waterflow, 70, 98)) + (lfNoise(20)*300) + (env*17);
someWater = os.osc(freq)*0.3*env:fi.lowpass(1, 700);
stream = someWater+someWater+someWater+someWater*0.2;

vol = hslider("vol", 0, 0,1,0.01);


process = someWater*vol;