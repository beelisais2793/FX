// karplus_strong.dsp
//
// Slightly modified version of the 
// Karplus-Strong plucked string algorithm.
//
// see: 'Making Virtual Electric Guitars and Associated Effects Using Faust'
//		 (Smith, )
//
// - one-pole lowpass in the feedback
//
// Henrik von Coler
// 2020-06-07

import("all.lib"); 

////////////////////////////////////////////////////////////////////////////////
// Control parameters as horizonal sliders:
////////////////////////////////////////////////////////////////////////////////

freq = hslider("freq Hz", 50, 20, 1000, 1) : si.smoo; // Hz

// initial filter for the excitation noise
initial_filter = hslider("initial_filter Hz",1000,10,10000,1) : si.smoo; 
lop = hslider("lop Hz",1000,10,10000,1) : si.smoo;  

level = hslider("level", 1, 0, 10, 0.01);     
gate = button("gate");                     
gain = hslider("gain",  1, 0, 1, 0.01);     

////////////////////////////////////////////////////////////////////////////////
// processing elements:
////////////////////////////////////////////////////////////////////////////////

diffgtz(x) = (x-x') > 0;
decay(n,x) = x - (x>0)/n;
release(n) = + ~ decay(n);
trigger(n) = diffgtz : release(n) : > (0.0);
 

P = SR/freq;

// Resonator:
resonator = (+ : delay(4096, P) * gain) ~ si.smooth(1.0-2*(lop/ma.SR));

////////////////////////////////////////////////////////////////////////////////
// processing function:
////////////////////////////////////////////////////////////////////////////////


process = noise : si.smooth(1.0-2*(initial_filter/ma.SR)):*(level) 
				 : *(gate : trigger(P)): resonator <: _,_;