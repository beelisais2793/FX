// white_tone.dsp
//
// Henrik von Coler
// 2021-07-04

import("all.lib"); 

// Control parameters:
freq = hslider("freq Hz", 50, 20, 1000, 1) : si.smoo; // Hz
gate = button("gate");                     
 
// processing elements for excitation:
diffgtz(x) = (x-x') > 0;
decay(n,x) = x - (x>0)/n;
release(n) = + ~ decay(n);
trigger(n) = diffgtz : release(n) : > (0.0);
 
P = SR/freq;

// Resonator:
resonator = (+ : delay(4096, P) * gate) ~ _;

 // processing function:
process = noise : *(gate : trigger(P)): resonator <: _,_;