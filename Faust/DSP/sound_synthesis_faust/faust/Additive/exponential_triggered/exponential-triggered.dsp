// exponential.dsp
//
// Additive synthesizer with  
// exponential spectral decay
//
// - triggered
// - 
// - stereo output
//
// Henrik von Coler
// 2020-05-12

import("stdfaust.lib");

// define a fundamental frequency
f0 	  = hslider("FREQ", 100, 0.1, 1000, 0.1);

// define the number of partials
n_partial = 50;

trigger  = button("Trigger");


slope         = hslider("Slope", 1, 0.1, 7, 0.02);

slope_release = hslider("Slope Release", 1, 0, 2, 0.01);

amp_release   = hslider("AMP Release",0.5,0.01,5,0.01);

// control the splitting of partials over time
split         = hslider("Frequency Split",0,0,10,0.01);

// partial function 
partial(partCNT, t, s, sr, ar) = os.oscrs(f) * volume * gain
	// arguments
	with {

		   
		   f      = f0 * ( (partCNT+1) + split * (1-en.arfe(0, ar, 0,t)));

		   // global gain envelope 
		   gain   = en.arfe(0.01, ar, 0,t);			

		   // spectral slope is controlled by a temporal envelope 
		   s2     =  s* partCNT  * en.arfe(0.01, sr, 0,t);	
		   		
		   	
		   volume = 0.1 * exp(-s2);

	 	  };

// the processing function, 
// running 50 partials parallel
// summing them up and applying a global gain
process = par(i, n_partial,  partial(i, trigger, slope, slope_release, amp_release)) :>_ * hslider("Master Gain",0,0,1, 0.1) <: _,_;