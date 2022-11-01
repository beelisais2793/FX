// exponential.dsp
//
// Additive synthesizer with controllable 
// exponential spectral decay.
//
// - continuous
// - stereo output
//
// Henrik von Coler
// 2020-05-05

import("stdfaust.lib");

// define a fundamental frequency
f0 	  = 100;

// define the number of partials
n_partial = 50;

slope     = hslider("s", 1, 0.1, 7, 0.01);


// partial function 
partial(partCNT,s) = os.oscrs(f) * volume
	// arguments
	with {
		   f = f0 * (partCNT+1);					
		   volume =  0.3 *  exp(s * -partCNT);
	 	  };

// the processing function, 
// running 50 partials parallel
// summing them up and applying a global gain
process = par(i, n_partial,  partial(i,slope)) :>_ * hslider("Master Gain",0,0,1, 0.1) <: _,_;