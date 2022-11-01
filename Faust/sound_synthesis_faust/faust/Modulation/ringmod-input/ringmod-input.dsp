// ringmod-input.dsp
//
// Ringmodulator for audio input
//
// - fader for controlling modulator frequency
// - fader for controlling mix of ringmod
//
// Henrik von Coler
// 2020-05-12

import("stdfaust.lib");
 
f_m     = hslider("Modulator Frequency",100,0.01,1000,0.1);
 
mix     = hslider("Modulation Mix",0.5,0,1,0.01);

am(x, fm) =  (1-mix) * x  +  mix * x *  os.osc(fm);
 
process(x) =     am(x,f_m) <: _,_;